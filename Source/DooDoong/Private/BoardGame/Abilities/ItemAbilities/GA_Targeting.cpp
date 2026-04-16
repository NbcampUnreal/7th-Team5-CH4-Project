#include "BoardGame/Abilities/ItemAbilities/GA_Targeting.h"

#include "System/DDGameplayTags.h"

UGA_Targeting::UGA_Targeting()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	AbilityTags.AddTag(DDGameplayTags::ItemAction_Ability_Targeting);
	ActivationBlockedTags.AddTag(DDGameplayTags::State_Character_Death);
}
