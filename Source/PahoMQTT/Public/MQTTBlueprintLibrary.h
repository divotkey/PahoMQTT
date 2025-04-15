// Copyright 2024, 2025 Roman Divotkey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MQTTSubsystem.h"
#include "MQTTBlueprintLibrary.generated.h"


/**
 * UMQTTBlueprintLibrary provides a convenient interface for MQTT operations in Blueprints.
 * It allows publishing messages, subscribing to topics, and checking connection status
 * without requiring direct references to the UMQTTSubsystem in Blueprints.
 * This simplifies MQTT integration for Blueprint users.
 */
UCLASS()
class PAHOMQTT_API UMQTTBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Publishes a message to a specified MQTT topic.
	 * @param Topic The topic to publish the message to.
	 * @param Message The message to publish.
	 * @param QoS The Quality of Service level (default is 1).
	 * @param Retain Whether to retain the message on the broker (default is false).
	 */
	UFUNCTION(BlueprintCallable, Category = "MQTT", meta = (WorldContext = "ContextObject", DisplayName = "Publish Message", ToolTip = "Publishes a message to a specified MQTT topic."))
	static void PublishMessage(UObject* ContextObject, const FString& Topic, const FString& Message, int QoS = 1, bool Retain = false);

	/**
	 * Subscribes to a specified MQTT topic.
	 * @param Topic The topic to subscribe to.
	 * @param QoS The Quality of Service level (default is 1).
	 */
	UFUNCTION(BlueprintCallable, Category = "MQTT", meta = (WorldContext = "ContextObject", DisplayName = "Subscribe to Topic", ToolTip = "Subscribes to a specified MQTT topic."))
	static void SubscribeToTopic(UObject* ContextObject, const FString& Topic, int QoS = 1);

	/**
	 * Unsubscribes from a specified MQTT topic.
	 * @param Topic The topic to unsubscribe from.
	 */
	UFUNCTION(BlueprintCallable, Category = "MQTT", meta = (WorldContext = "ContextObject", DisplayName = "Unsubscribe from Topic", ToolTip = "Unsubscribes from a specified MQTT topic."))
	static void UnsubscribeFromTopic(UObject* ContextObject, const FString& Topic);

	/**
	 * Checks if the MQTT Subsystem is currently connected to the broker.
	 * @return True if connected, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category = "MQTT", meta = (WorldContext = "ContextObject", DisplayName = "Is Connected", ToolTip = "Checks if the MQTT subsystem is currently connected."))
	static bool IsConnected(UObject* ContextObject);

private:

	/**
	 * Gets the MQTT subsystem.
	 * @param ContextObject The context object to get the world from.
	 * @return The MQTT subsystem.
	 */
	static UMQTTSubsystem* GetMQTTSubsystem(UObject* ContextObject);

};
