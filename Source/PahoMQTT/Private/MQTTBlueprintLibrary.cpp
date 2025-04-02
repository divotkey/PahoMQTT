// Copyright 2024, 2025 Roman Divotkey. All Rights Reserved.


#include "MQTTBlueprintLibrary.h"
#include "PahoMQTT.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"


UMQTTSubsystem* UMQTTBlueprintLibrary::GetMQTTSubsystem(UObject* ContextObject)
{
    check(ContextObject);

    if (UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::LogAndReturnNull)) {
        UGameInstance* GameInstance = World->GetGameInstance();
        if (GameInstance) {
            return GameInstance->GetSubsystem<UMQTTSubsystem>();
        }
    }

    UE_LOG(LogMQTT, Error, TEXT("Unable to access MQTTSubsystem Subsystem"));
    return nullptr;
}

void UMQTTBlueprintLibrary::PublishMessage(UObject* ContextObject, const FString& Topic, const FString& Message, int QoS, bool Retain)
{
    UMQTTSubsystem* MQTTSubsystem = GetMQTTSubsystem(ContextObject);
    if (MQTTSubsystem)
    {
        MQTTSubsystem->PublishMessage(Topic, Message, QoS, Retain);
    }
}

void UMQTTBlueprintLibrary::SubscribeToTopic(UObject* ContextObject, const FString& Topic, int QoS)
{
    UMQTTSubsystem* MQTTSubsystem = GetMQTTSubsystem(ContextObject);
    if (MQTTSubsystem)
    {
        UE_LOG(LogMQTT, Log, TEXT("Subscribing to topic: %s"), *Topic);
        MQTTSubsystem->SubscribeToTopic(Topic, QoS);
    }
}

void UMQTTBlueprintLibrary::UnsubscribeFromTopic(UObject* ContextObject, const FString& Topic)
{
    UMQTTSubsystem* MQTTSubsystem = GetMQTTSubsystem(ContextObject);
    if (MQTTSubsystem)
    {
        MQTTSubsystem->UnsubscribeFromTopic(Topic);
    }
}

bool UMQTTBlueprintLibrary::IsConnected(UObject* ContextObject)
{
    UMQTTSubsystem* MQTTSubsystem = GetMQTTSubsystem(ContextObject);
    if (MQTTSubsystem)
    {
        return MQTTSubsystem->IsConnected();
    }
    return false;
}
