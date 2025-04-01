// Copyright 2024, 2025 Roman Divotkey. All Rights Reserved.


#include "MQTTSubsystem.h"
#include "PahoMQTTRuntimeSettings.h"
#include "PahoMQTT.h"


void UMQTTSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogMQTT, Display, TEXT("Initializing MQTT Subsystem"));
	Super::Initialize(Collection);


	const UPahoMQTTRuntimeSettings* Settings = GetDefault<UPahoMQTTRuntimeSettings>();
	UE_LOG(LogMQTT, Display, TEXT("MQTT Broker Address: %s"), *Settings->BrokerAddress);
	UE_LOG(LogMQTT, Display, TEXT("MQTT Client ID: %s"), *Settings->ClientID);

	if (Settings->bAutoConnect) {
		UE_LOG(LogMQTT, Display, TEXT("Auto-connecting to MQTT Broker as configured in project settings"));

		SimpleMQTTClient = NewObject<USimpleMQTTClient>(this);
		SimpleMQTTClient->InitializeClient(Settings->BrokerAddress, Settings->ClientID);

		SimpleMQTTClient->OnConnected.AddDynamic(this, &UMQTTSubsystem::HandleMQTTConnected);
		SimpleMQTTClient->OnMessageReceived.AddDynamic(this, &UMQTTSubsystem::HandleMQTTMessageReceived);
		SimpleMQTTClient->OnConnectionLost.AddDynamic(this, &UMQTTSubsystem::HandleMQTTConnectionLost);
		SimpleMQTTClient->OnDisconnected.AddDynamic(this, &UMQTTSubsystem::HandleMQTTDisconnected);

		SimpleMQTTClient->Connect();
	}
}

void UMQTTSubsystem::Deinitialize()
{
	UE_LOG(LogMQTT, Display, TEXT("Deinitializing MQTT Subsystem"));
	Super::Deinitialize();

	if (SimpleMQTTClient != nullptr) {
		SimpleMQTTClient->ShutdownClient();
		SimpleMQTTClient = nullptr;
	}

	// Clear the list of subscribed topics
	SubscribedTopics.Empty();
}

bool UMQTTSubsystem::IsConnected() const
{
	return SimpleMQTTClient != nullptr && SimpleMQTTClient->IsConnected();
}

void UMQTTSubsystem::PublishMessage(const FString& Topic, const FString& Message, int QoS, bool Retain)
{
	if (SimpleMQTTClient != nullptr && SimpleMQTTClient->IsConnected()) {
		SimpleMQTTClient->PublishMessage(Topic, Message, QoS, Retain);
	}
}

void UMQTTSubsystem::SubscribeToTopic(const FString& Topic, int QoS)
{
	SubscribedTopics.Add(TPair<FString, int>(Topic, QoS));

	if (SimpleMQTTClient != nullptr && SimpleMQTTClient->IsConnected()) {
		SimpleMQTTClient->SubscribeTopic(Topic, QoS);
	}
}

void UMQTTSubsystem::UnsubscribeFromTopic(const FString& Topic)
{
	// Remove the topic from the list of subscribed topics
	SubscribedTopics.RemoveAll([&](const TPair<FString, int>& Pair) {
		return Pair.Key == Topic;
		});

	if (SimpleMQTTClient != nullptr) {
		SimpleMQTTClient->UnsubscribeTopic(Topic);
	}
}

void UMQTTSubsystem::HandleMQTTConnected()
{
	UE_LOG(LogMQTT, Display, TEXT("Successfully connected to MQTT Broker"));	
	OnMQTTConnected.Broadcast();
	ResubscribeToAllTopics();
}

void UMQTTSubsystem::HandleMQTTMessageReceived(const FString& Topic, const FString& Message)
{
	OnMQTTMessageReceived.Broadcast(Topic, Message);
}

void UMQTTSubsystem::HandleMQTTConnectionLost(const FString& Cause)
{
	UE_LOG(LogMQTT, Display, TEXT("MQTT Connection Lost: %s"), *Cause);
	OnMQTTConnectionLost.Broadcast(Cause);
}

void UMQTTSubsystem::HandleMQTTDisconnected()
{
	UE_LOG(LogMQTT, Display, TEXT("Disconnected from MQTT Broker"));
	OnMQTTDisconnected.Broadcast();
}

void UMQTTSubsystem::ResubscribeToAllTopics()
{
	// Resubscribe to all topics in the list
	check(SimpleMQTTClient != nullptr);
	check(SimpleMQTTClient->IsConnected());
	for (const TPair<FString, int>& TopicPair : SubscribedTopics) {
		SimpleMQTTClient->SubscribeTopic(TopicPair.Key, TopicPair.Value);
	}
}
