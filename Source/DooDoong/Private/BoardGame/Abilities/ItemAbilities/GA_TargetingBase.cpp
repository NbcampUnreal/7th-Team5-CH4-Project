#include "BoardGame/Abilities/ItemAbilities/GA_TargetingBase.h"

#include "System/DDGameplayTags.h"

UGA_TargetingBase::UGA_TargetingBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	AbilityTags.AddTag(DDGameplayTags::ItemAction_Ability_Targeting);
	ActivationBlockedTags.AddTag(DDGameplayTags::State_Character_Death);
}
