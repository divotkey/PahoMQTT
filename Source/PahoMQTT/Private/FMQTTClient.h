// Copyright 2024, 2025 Roman Divotkey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "Containers/Queue.h"
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "MQTTAsync.h"

// Event-Delegates
DECLARE_DELEGATE(FOnConnectedDelegate);
DECLARE_DELEGATE_TwoParams(FOnMessageReceivedDelegate, FString /*Topic*/, FString /*Message*/);
DECLARE_DELEGATE_OneParam(FOnConnectionLostDelegate, FString /*Cause*/);
DECLARE_DELEGATE(FOnDisconnectedDelegate);

class FMQTTClient
{
public:
    FMQTTClient();
    ~FMQTTClient();

    void Initialize(const FString& BrokerAddress, const FString& ClientID);
    void Connect();
    void Disconnect();
    void Shutdown();

    void PublishMessage(const FString& Topic, const FString& Message, int QoS = 1, bool Retain = false);
    void SubscribeTopic(const FString& Topic, int QoS = 1);
    void UnsubscribeTopic(const FString& Topic);

    // Check if the client is connected
    bool IsConnected() const;

    // Event-Delegates
    FOnConnectedDelegate OnConnected;
    FOnMessageReceivedDelegate OnMessageReceived;
    FOnConnectionLostDelegate OnConnectionLost;
    FOnDisconnectedDelegate OnDisconnected;

private:
    MQTTAsync Client;
    MQTTAsync_connectOptions ConnOpts;
    MQTTAsync_disconnectOptions DisconnOpts;

    mutable std::mutex Mutex;
    std::condition_variable ConditionVariable;
    std::atomic<bool> bIsShuttingDown;
    bool bIsDisconnected;

    // Callbacks
    static void ConnectionLost(void* context, char* cause);
    static int MessageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message);
    static void OnConnect(void* context, MQTTAsync_successData* response);
    static void OnConnectFailure(void* context, MQTTAsync_failureData* response);
    static void OnDisconnect(void* context, MQTTAsync_successData* response);
    static void OnDisconnectFailure(void* context, MQTTAsync_failureData* response);
    static void OnPublish(void* context, MQTTAsync_successData* response);
    static void OnPublishFailure(void* context, MQTTAsync_failureData* response);
};
