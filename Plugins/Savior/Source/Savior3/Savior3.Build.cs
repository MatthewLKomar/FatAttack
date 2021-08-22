// Copyright 2020 (C) Bruno Xavier Leite

using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules {
//
public class Savior3 : ModuleRules {
    public Savior3(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Public/Savior3_Shared.h";
		bEnforceIWYU = true;
		//
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"UMG",
				"Core",
				"Json",
				"Slate",
				"Engine",
				"SlateCore",
				"CoreUObject",
				"MoviePlayer",
				"GameplayTags",
				"JsonUtilities"
			}///
		);//
		//
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory,"Public"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory,"Private"));
	}///
}///
//
}