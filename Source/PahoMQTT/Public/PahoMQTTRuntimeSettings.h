// Copyright 2024, 2025 Roman Divotkey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PahoMQTTRuntimeSettings.generated.h"

/**
 * Runtime settings for the Varjo PahoMQTT Plugin.
 *
 * This class holds configuration settings for the PahoMQTT plugin. These settings
 * are accessible and editable within the Project Settings in the Unreal Editor, allowing
 * for easy customization of the toolkit's behavior.
 */
UCLASS(config = Game, defaultconfig)
class PAHOMQTT_API UPahoMQTTRuntimeSettings : public UObject
{
	GENERATED_BODY()

public:

	UPahoMQTTRuntimeSettings();

	// Specifies whether to automatically connect to the specified MQTT broker
	UPROPERTY(Config, EditAnywhere, Category = "MQTT", meta = (DisplayName = "Auto Connect To MQTT Broker"))
	bool bAutoConnect;

	// The address of the MQTT broker
	UPROPERTY(Config, EditAnywhere, Category = "MQTT", meta = (DisplayName = "MQTT Broker Address"))
	FString BrokerAddress;

	// The client ID to use when connecting to the MQTT broker
	UPROPERTY(Config, EditAnywhere, Category = "MQTT", meta = (DisplayName = "MQTT Client ID"))
	FString ClientID;
};
