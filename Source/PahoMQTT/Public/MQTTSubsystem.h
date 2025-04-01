// Copyright 2024, 2025 Roman Divotkey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SimpleMQTTClient.h"
#include "MQTTSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PAHOMQTT_API UMQTTSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMQTTConnected);
	UPROPERTY(BlueprintAssignable, Category = "MQTT")
	FOnMQTTConnected OnMQTTConnected;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMQTTMessageReceived, const FString&, Topic, const FString&, Message);
	UPROPERTY(BlueprintAssignable, Category = "MQTT")
	FOnMQTTMessageReceived OnMQTTMessageReceived;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMQTTConnectionLost, const FString&, Cause);
	UPROPERTY(BlueprintAssignable, Category = "MQTT")
	FOnMQTTConnectionLost OnMQTTConnectionLost;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMQTTDisconnected);
	UPROPERTY(BlueprintAssignable, Category = "MQTT")
	FOnMQTTDisconnected OnMQTTDisconnected;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Checks if the MQTT client is currently connected.
	 * @return True if connected, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "MQTT", meta = (DisplayName = "Is Connected", ToolTip = "Checks if the MQTT client is currently connected."))
	bool IsConnected() const;

	/**
	 * Publishes a message to a specified MQTT topic.
	 * @param Topic The topic to publish the message to.
	 * @param Message The message to publish.
	 * @param QoS The Quality of Service level (default is 1).
	 * @param Retain Whether to retain the message on the broker (default is false).
	 */
	UFUNCTION(BlueprintCallable, Category = "MQTT", meta = (DisplayName = "Publish Message", ToolTip = "Publishes a message to a specified MQTT topic."))
	void PublishMessage(const FString& Topic, const FString& Message, int QoS = 1, bool Retain = false);

	/**
	 * Subscribes to a specified MQTT topic.
	 * @param Topic The topic to subscribe to.
	 * @param QoS The Quality of Service level (default is 1).
	 */
	UFUNCTION(BlueprintCallable, Category = "MQTT", meta = (DisplayName = "Subscribe to Topic", ToolTip = "Subscribes to a specified MQTT topic."))
	void SubscribeToTopic(const FString& Topic, int QoS = 1);

	/**
	 * Unsubscribes from a specified MQTT topic.
	 * @param Topic The topic to unsubscribe from.
	 */
	UFUNCTION(BlueprintCallable, Category = "MQTT", meta = (DisplayName = "Unsubscribe from Topic", ToolTip = "Unsubscribes from a specified MQTT topic."))
	void UnsubscribeFromTopic(const FString& Topic);

private:
	UPROPERTY();
	USimpleMQTTClient* SimpleMQTTClient;

	// List to store subscribed topics
	TArray<TPair<FString, int>> SubscribedTopics;

	// Eventhandler for MQTT client events
	UFUNCTION()
	void HandleMQTTConnected();
	UFUNCTION()
	void HandleMQTTMessageReceived(const FString& Topic, const FString& Message);
	UFUNCTION()
	void HandleMQTTConnectionLost(const FString& Cause);
	UFUNCTION()
	void HandleMQTTDisconnected();


	// Helper function to resubscribe to all topics
	void ResubscribeToAllTopics();
};
