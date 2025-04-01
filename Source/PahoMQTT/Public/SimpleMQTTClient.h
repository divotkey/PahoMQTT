// Copyright 2024, 2025 Roman Divotkey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MQTTAsync.h"
#include "SimpleMQTTClient.generated.h"

// Declaration of delegates for events
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMQTTConnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMQTTConnectionLost, const FString&, Cause);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMQTTMessageReceived, const FString&, Topic, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMQTTSendSuccess, int, Token);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMQTTSendFailure, int, Token, int, ErrorCode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMQTTDisconnected);

class FMQTTClient;

UCLASS(Blueprintable)
class PAHOMQTT_API USimpleMQTTClient : public UObject
{
    GENERATED_BODY()

public:
    USimpleMQTTClient();
    virtual ~USimpleMQTTClient();

    UFUNCTION(BlueprintCallable, Category = "MQTT")
    void InitializeClient(const FString& BrokerAddress, const FString& ClientID);

    UFUNCTION(BlueprintCallable, Category = "MQTT")
    void Connect();

    UFUNCTION(BlueprintCallable, Category = "MQTT")
    void Disconnect();

    UFUNCTION(BlueprintCallable, Category = "MQTT")
    void PublishMessage(const FString& Topic, const FString& Message, int QoS = 1, bool Retain = false);

    UFUNCTION(BlueprintCallable, Category = "MQTT")
    void SubscribeTopic(const FString& Topic, int QoS = 1);

    UFUNCTION(BlueprintCallable, Category = "MQTT")
    void UnsubscribeTopic(const FString& Topic);

    UFUNCTION(BlueprintCallable, Category = "MQTT")
    void ShutdownClient();

    // Check if the client is connected
    UFUNCTION(BlueprintCallable, Category = "MQTT")
    bool IsConnected() const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "MQTT")
    FOnMQTTConnected OnConnected;

    UPROPERTY(BlueprintAssignable, Category = "MQTT")
    FOnMQTTMessageReceived OnMessageReceived;

    UPROPERTY(BlueprintAssignable, Category = "MQTT")
    FOnMQTTConnectionLost OnConnectionLost;

    UPROPERTY(BlueprintAssignable, Category = "MQTT")
    FOnMQTTDisconnected OnDisconnected;

private:
    TSharedPtr<FMQTTClient> MQTTClientImpl;

    // Event handler
    void HandleConnected();
    void HandleMessageReceived(FString Topic, FString Message);
    void HandleConnectionLost(FString Cause);
    void HandleDisconnected();
};
