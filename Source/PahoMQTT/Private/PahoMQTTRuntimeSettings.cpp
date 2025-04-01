// Copyright 2024, 2025 Roman Divotkey. All Rights Reserved.


#include "PahoMQTTRuntimeSettings.h"

UPahoMQTTRuntimeSettings::UPahoMQTTRuntimeSettings()
	: bAutoConnect(false)
	, BrokerAddress(TEXT("mqtt://localhost:1883"))
	, ClientID(TEXT("UnrealMQTTClient"))
{
	// Intentionally left empty.
}
