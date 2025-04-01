// Copyright 2024, 2025 Roman Divotkey. All Rights Reserved.

#include "PahoMQTT.h"
#include "PahoMQTTRuntimeSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FPahoMQTTModule"

DEFINE_LOG_CATEGORY(LogMQTT);

void FPahoMQTTModule::StartupModule()
{
	// Attempt to get the Settings Module to register our plugin settings within the
	// Unreal Editor. This allows users to configure the Varjo Hub Toolkit plugin 
	// settings directly from the Project Settings under the "Plugins" category.	

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "PahoMQTT",
			LOCTEXT("RuntimeSettingsName", "Paho MQTT"),
			LOCTEXT("RuntimeSettingsDescription", "Settings for Paho MQTT Plugin"),
			GetMutableDefault<UPahoMQTTRuntimeSettings>()
		);
	}
}

void FPahoMQTTModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPahoMQTTModule, PahoMQTT)
