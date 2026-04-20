#include "BoardGame/Abilities/ItemAbilities/ItemBase/GA_InstantItemBase.h"

void UGA_InstantItemBase::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthority(&ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	ExecuteItemCue(ConfirmCueTag);

	const bool bExecuted = ExecuteInstantItem(TriggerEventData);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, !bExecuted);
}

bool UGA_InstantItemBase::ExecuteInstantItem(const FGameplayEventData* TriggerEventData)
{
	return false;
}
