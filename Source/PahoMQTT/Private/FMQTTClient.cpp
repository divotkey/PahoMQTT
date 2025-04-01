// Copyright 2024, 2025 Roman Divotkey. All Rights Reserved.

#include "FMQTTClient.h"
#include "PahoMQTT.h"
#include "Async/Async.h"

FMQTTClient::FMQTTClient()
	: Client(nullptr)
	, bIsShuttingDown(false)
	, bIsDisconnected(true)
{
	FMemory::Memzero(&ConnOpts, sizeof(ConnOpts));
	ConnOpts = MQTTAsync_connectOptions_initializer;
	ConnOpts.keepAliveInterval = 20;
	ConnOpts.cleansession = 1;
	ConnOpts.onSuccess = &FMQTTClient::OnConnect;
	ConnOpts.onFailure = &FMQTTClient::OnConnectFailure;
	ConnOpts.context = this;

	FMemory::Memzero(&DisconnOpts, sizeof(DisconnOpts));
	DisconnOpts = MQTTAsync_disconnectOptions_initializer;
	DisconnOpts.onSuccess = &FMQTTClient::OnDisconnect;
	DisconnOpts.onFailure = &FMQTTClient::OnDisconnectFailure;
	DisconnOpts.context = this;
}

FMQTTClient::~FMQTTClient()
{
	Shutdown();
}

void FMQTTClient::Initialize(const FString& BrokerAddress, const FString& ClientID)
{
	const char* address = TCHAR_TO_UTF8(*BrokerAddress);
	const char* clientId = TCHAR_TO_UTF8(*ClientID);

	int rc = MQTTAsync_create(&Client, address, clientId, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	if (rc != MQTTASYNC_SUCCESS)
	{
		UE_LOG(LogMQTT, Error, TEXT("Failed to create MQTT client. Error code: %d"), rc);
		return;
	}

	rc = MQTTAsync_setCallbacks(Client, this, &FMQTTClient::ConnectionLost, &FMQTTClient::MessageArrived, NULL);
	if (rc != MQTTASYNC_SUCCESS)
	{
		UE_LOG(LogMQTT, Error, TEXT("Failed to set MQTT callbacks. Error code: %d"), rc);
		return;
	}
}

void FMQTTClient::Connect()
{
	if (Client == nullptr)
	{
		UE_LOG(LogMQTT, Error, TEXT("MQTT Client not initialized."));
		return;
	}


	int rc = MQTTAsync_connect(Client, &ConnOpts);
	if (rc != MQTTASYNC_SUCCESS)
	{
		UE_LOG(LogMQTT, Error, TEXT("Failed to start connect. Error code: %d"), rc);
		return;
	}
}

bool FMQTTClient::IsConnected() const
{
	std::unique_lock<std::mutex> lock(Mutex);
	return !bIsDisconnected;
}

void FMQTTClient::Disconnect()
{
	if (Client != nullptr && !bIsShuttingDown)
	{
		int rc = MQTTAsync_disconnect(Client, &DisconnOpts);
		if (rc != MQTTASYNC_SUCCESS)
		{
			UE_LOG(LogMQTT, Error, TEXT("Failed to start disconnect. Error code: %d"), rc);
		}
	}
}

void FMQTTClient::Shutdown()
{
	if (Client != nullptr && !bIsShuttingDown.load())
	{
		bIsShuttingDown.store(true);

		// Trennen der Verbindung
		MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
		opts.onSuccess = &FMQTTClient::OnDisconnect;
		opts.onFailure = &FMQTTClient::OnDisconnectFailure;
		opts.context = this;

		int rc = MQTTAsync_disconnect(Client, &opts);
		if (rc != MQTTASYNC_SUCCESS)
		{
			UE_LOG(LogMQTT, Error, TEXT("Failed to start disconnect. Error code: %d"), rc);
			// Direkte Zerstörung, wenn die Trennung fehlschlägt
			MQTTAsync_destroy(&Client);
			Client = nullptr;
		}
		else
		{
			// Warten auf die Trennung
			std::unique_lock<std::mutex> lock(Mutex);
			ConditionVariable.wait(lock, [this]() { return bIsDisconnected; });

			MQTTAsync_destroy(&Client);
			Client = nullptr;
		}
	}
}

void FMQTTClient::PublishMessage(const FString& Topic, const FString& Message, int QoS, bool Retain)
{
	if (Client == nullptr)
	{
		UE_LOG(LogMQTT, Error, TEXT("MQTT Client not connected."));
		return;
	}

	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	opts.onSuccess = &FMQTTClient::OnPublish;
	opts.onFailure = &FMQTTClient::OnPublishFailure;
	opts.context = this;

	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
	std::string payload = TCHAR_TO_UTF8(*Message);
	pubmsg.payload = (void*)payload.c_str();
	pubmsg.payloadlen = static_cast<int>(payload.length());
	pubmsg.qos = QoS;
	pubmsg.retained = Retain;

	int rc = MQTTAsync_sendMessage(Client, TCHAR_TO_UTF8(*Topic), &pubmsg, &opts);
	if (rc != MQTTASYNC_SUCCESS)
	{
		UE_LOG(LogMQTT, Error, TEXT("Failed to start sendMessage. Error code: %d"), rc);
		return;
	}
}

void FMQTTClient::SubscribeTopic(const FString& Topic, int QoS)
{
	if (Client == nullptr)
	{
		UE_LOG(LogMQTT, Error, TEXT("MQTT Client not connected."));
		return;
	}


	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	opts.context = this;

	int rc = MQTTAsync_subscribe(Client, TCHAR_TO_UTF8(*Topic), QoS, &opts);
	if (rc != MQTTASYNC_SUCCESS)
	{
		UE_LOG(LogMQTT, Error, TEXT("Failed to start subscribe. Error code: %d"), rc);
		return;
	}
}

void FMQTTClient::UnsubscribeTopic(const FString& Topic)
{
	if (Client == nullptr)
	{
		UE_LOG(LogMQTT, Error, TEXT("MQTT Client not initialized."));
		return;
	}

	const char* topic = TCHAR_TO_UTF8(*Topic);
	int rc = MQTTAsync_unsubscribe(Client, topic, nullptr);
	if (rc != MQTTASYNC_SUCCESS)
	{
		UE_LOG(LogMQTT, Error, TEXT("Failed to unsubscribe from topic. Error code: %d"), rc);
	}
}

// Callback implementations
void FMQTTClient::ConnectionLost(void* context, char* cause)
{
	FMQTTClient* self = static_cast<FMQTTClient*>(context);
	if (self)
	{
		std::lock_guard<std::mutex> lock(self->Mutex);
		self->bIsDisconnected = true;

		FString Cause = FString(UTF8_TO_TCHAR(cause ? cause : "Unknown"));

		// Ensure that the game thread is used
		AsyncTask(ENamedThreads::GameThread, [self, Cause]()
			{
				self->OnConnectionLost.ExecuteIfBound(Cause);
			});
	}
}

int FMQTTClient::MessageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message)
{
	FMQTTClient* self = static_cast<FMQTTClient*>(context);
	if (self)
	{
		if (!self->bIsShuttingDown.load()) {
			FString Topic = FString(UTF8_TO_TCHAR(topicName));
			// Processing the payload
			int payloadlen = message->payloadlen;
			const char* payloadData = static_cast<const char*>(message->payload);

			FString Payload;
			if (payloadlen > 0 && payloadData != nullptr)
			{
				// Use of FUTF8ToTCHAR to convert the payload into an FString
				FUTF8ToTCHAR Converter(payloadData, payloadlen);
				Payload = FString(Converter.Length(), Converter.Get());
			}
			else
			{
				Payload = FString();
			}
			// Ensure that the game thread is used
			AsyncTask(ENamedThreads::GameThread, [self, Topic, Payload]()
				{
					self->OnMessageReceived.ExecuteIfBound(Topic, Payload);
				});
		}
	}

	MQTTAsync_freeMessage(&message);
	MQTTAsync_free(topicName);
	return 1;
}

void FMQTTClient::OnConnect(void* context, MQTTAsync_successData* response)
{
	FMQTTClient* self = static_cast<FMQTTClient*>(context);
	if (self)
	{
		{
			std::lock_guard<std::mutex> lock(self->Mutex);
			self->bIsDisconnected = false;
		}

		// Ensure that the game thread is used
		AsyncTask(ENamedThreads::GameThread, [self]()
			{
				self->OnConnected.ExecuteIfBound();
			});
	}
}

void FMQTTClient::OnConnectFailure(void* context, MQTTAsync_failureData* response)
{
	FMQTTClient* self = static_cast<FMQTTClient*>(context);
	if (self)
	{
		UE_LOG(LogMQTT, Error, TEXT("MQTT Connection failed. Error code: %d"), response ? response->code : 0);
	}
}

void FMQTTClient::OnDisconnect(void* context, MQTTAsync_successData* response)
{
	FMQTTClient* self = static_cast<FMQTTClient*>(context);
	if (self)
	{

		if (!!self->bIsShuttingDown.load()) {
			// Ensure that the game thread is used
			AsyncTask(ENamedThreads::GameThread, [self]()
				{
					self->OnDisconnected.ExecuteIfBound();
				});
		}

		{
			std::lock_guard<std::mutex> lock(self->Mutex);
			self->bIsDisconnected = true;
		}
		self->ConditionVariable.notify_one();
	}
}

void FMQTTClient::OnDisconnectFailure(void* context, MQTTAsync_failureData* response)
{
	FMQTTClient* self = static_cast<FMQTTClient*>(context);
	if (self)
	{
		UE_LOG(LogMQTT, Error, TEXT("MQTT Disconnect failed. Error code: %d"), response ? response->code : 0);

		// If necessary, mark the operation as completed
		{
			std::lock_guard<std::mutex> lock(self->Mutex);
			self->bIsDisconnected = true;
		}
		self->ConditionVariable.notify_one();
	}
}

void FMQTTClient::OnPublish(void* context, MQTTAsync_successData* response)
{
	FMQTTClient* self = static_cast<FMQTTClient*>(context);
	if (self)
	{
		
		UE_LOG(LogMQTT, Verbose, TEXT("MQTT Message successfully published"));
	}
}

void FMQTTClient::OnPublishFailure(void* context, MQTTAsync_failureData* response)
{
	FMQTTClient* self = static_cast<FMQTTClient*>(context);
	if (self)
	{
		UE_LOG(LogMQTT, Error, TEXT("MQTT Publish failed. Error code: %d"), response ? response->code : 0);
	}
}


