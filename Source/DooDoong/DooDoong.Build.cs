using UnrealBuildTool;

public class DooDoong : ModuleRules
{
	public DooDoong(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange([ModuleDirectory]);
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine",

			"InputCore",
			"EnhancedInput",

			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags",
			/* 레벨 시퀀서 */
			"LevelSequence", "MovieScene"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
			"UMG"
		});
	}
}
