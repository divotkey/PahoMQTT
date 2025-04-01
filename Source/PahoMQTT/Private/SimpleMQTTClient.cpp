// Copyright 2024, 2025 Roman Divotkey. All Rights Reserved.

#include "SimpleMQTTClient.h"
#include "FMQTTClient.h"


USimpleMQTTClient::USimpleMQTTClient()
{
    MQTTClientImpl = MakeShared<FMQTTClient>();
}

USimpleMQTTClient::~USimpleMQTTClient()
{
    ShutdownClient();
}

void USimpleMQTTClient::InitializeClient(const FString& BrokerAddress, const FString& ClientID)
{
    if (MQTTClientImpl.IsValid())
    {
        MQTTClientImpl->Initialize(BrokerAddress, ClientID);

        // Bind event handler
        MQTTClientImpl->OnConnected.BindUObject(this, &USimpleMQTTClient::HandleConnected);
        MQTTClientImpl->OnMessageReceived.BindUObject(this, &USimpleMQTTClient::HandleMessageReceived);
        MQTTClientImpl->OnConnectionLost.BindUObject(this, &USimpleMQTTClient::HandleConnectionLost);
        MQTTClientImpl->OnDisconnected.BindUObject(this, &USimpleMQTTClient::HandleDisconnected);
    }
}

void USimpleMQTTClient::Connect()
{
    if (MQTTClientImpl.IsValid())
    {
        MQTTClientImpl->Connect();
    }
}

void USimpleMQTTClient::Disconnect()
{
    if (MQTTClientImpl.IsValid())
    {
        MQTTClientImpl->Disconnect();
    }
}

void USimpleMQTTClient::PublishMessage(const FString& Topic, const FString& Message, int QoS, bool Retain)
{
    if (MQTTClientImpl.IsValid())
    {
        MQTTClientImpl->PublishMessage(Topic, Message, QoS, Retain);
    }
}

void USimpleMQTTClient::SubscribeTopic(const FString& Topic, int QoS)
{
    if (MQTTClientImpl.IsValid())
    {
        MQTTClientImpl->SubscribeTopic(Topic, QoS);
    }
}

void USimpleMQTTClient::UnsubscribeTopic(const FString& Topic)
{
    if (MQTTClientImpl.IsValid())
    {
        MQTTClientImpl->UnsubscribeTopic(Topic);
    }
}

void USimpleMQTTClient::ShutdownClient()
{
    if (MQTTClientImpl.IsValid())
    {
        MQTTClientImpl->Shutdown();
        MQTTClientImpl.Reset();
    }
}

bool USimpleMQTTClient::IsConnected() const
{
    if (MQTTClientImpl.IsValid())
    {
        return MQTTClientImpl->IsConnected();
    }
    return false;
}

// Event handler
void USimpleMQTTClient::HandleConnected()
{
    OnConnected.Broadcast();
}

void USimpleMQTTClient::HandleMessageReceived(FString Topic, FString Message)
{
    OnMessageReceived.Broadcast(Topic, Message);
}

void USimpleMQTTClient::HandleConnectionLost(FString Cause)
{
    OnConnectionLost.Broadcast(Cause);
}

void USimpleMQTTClient::HandleDisconnected()
{
    OnDisconnected.Broadcast();
}
