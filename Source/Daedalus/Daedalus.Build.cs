using UnrealBuildTool;

public class Daedalus : ModuleRules
{
	public Daedalus(TargetInfo Target) {
		MinFilesUsingPrecompiledHeaderOverride = 1;
		bFasterWithoutUnity = true;
		bUseRTTI = true;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore",
			"RHI", "RenderCore", "ShaderCore"
		});
		PrivateDependencyModuleNames.AddRange(new string[] { });
		
		PrivateIncludePaths.AddRange(new string[]
			{
				"Daedalus"
			}
		);

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] {
			"Slate", "SlateCore"
		});
		
		// Uncomment if you are using online features
		PrivateDependencyModuleNames.Add("OnlineSubsystem");
		if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64)) {
			if (UEBuildConfiguration.bCompileSteamOSS == true)
				DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		}
	}
}
