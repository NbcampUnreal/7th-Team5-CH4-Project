#include "BoardGame/Abilities/ItemAbilities/GA_GiveBomb.h"

#include "System/DDGameplayTags.h"

UGA_GiveBomb::UGA_GiveBomb()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	ActivationBlockedTags.AddTag(DDGameplayTags::State_Character_Death);
}
