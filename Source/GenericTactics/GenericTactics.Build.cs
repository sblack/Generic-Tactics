// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GenericTactics : ModuleRules
{
	public GenericTactics(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "Paper2D", "AssetRegistry" }); //, "HeadMountedDisplay"

        PublicDependencyModuleNames.AddRange(new string[] { "RHI", "RenderCore" }); //required for dynamic textures

        //needed for BindKey()
        PublicDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        PublicDependencyModuleNames.AddRange(new string[] { "NavigationSystem" });

        PublicDependencyModuleNames.AddRange(new string[] { "PaperZD", "PaperZDEditor" });

        if (Target.Type == TargetType.Editor)
        {
            PublicDependencyModuleNames.Add("PropertyEditor");
        }
    }
}
