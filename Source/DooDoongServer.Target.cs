// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DooDoongServerTarget : TargetRules
{
	public DooDoongServerTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		ExtraModuleNames.Add("DooDoong");

		GlobalDefinitions.Add("UE_PROJECT_STEAMPRODUCTNAME=\"480\"");
		GlobalDefinitions.Add("UE_PROJECT_STEAMGAMEDIR=\"DooDoong\"");
		GlobalDefinitions.Add("UE_PROJECT_STEAMGAMEDESC=\"DooDoong\"");
		GlobalDefinitions.Add("UE_PROJECT_STEAMSHIPPINGID=480");
	}
}
