// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class PahoMQTT : ModuleRules
{
	public PahoMQTT(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        //bEnableUndefinedIdentifierWarnings = false;

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
            }
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				Path.Combine(ModuleDirectory, "../../ThirdParty/PahoMQTT/Include")
            }
            );
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Projects"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);


        // Platform-specific settings
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // Path to the static libraries
            string PahoLibPath = Path.Combine(ModuleDirectory, "../../ThirdParty/PahoMQTT/Lib/Win64");

            PublicAdditionalLibraries.AddRange(
                new string[] {
                    Path.Combine(PahoLibPath, "paho-mqtt3a-static.lib"),
                }
            );

            PublicDefinitions.AddRange(
                new string[] {
                    "LIBMQTT_API=",
                    "PAHO_MQTT_STATIC=1"
                }
            );

        }


        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
