// Copyright 2020 (C) Bruno Xavier Leite

using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules {
//
public class Savior3Editor : ModuleRules {
    public Savior3Editor(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Public/Savior3Editor_Shared.h";
		bEnforceIWYU = true;
		//
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "Engine",
                "Savior3",
                "CoreUObject"
            }///
        );//
        //
        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Projects",
                "UnrealEd",
				"SlateCore",
                "AssetTools"
            }///
        );//
    }///
}///
//
}