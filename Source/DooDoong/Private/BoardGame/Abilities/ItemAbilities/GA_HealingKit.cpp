#include "BoardGame/Abilities/ItemAbilities/GA_HealingKit.h"

#include "AbilitySystemComponent.h"
#include "Animation/Notify/AN_SendGameplayEvent.h"
#include "Base/Player/DDBasePlayerState.h"
#include "Common/DDLogManager.h"
#include "System/DDGameplayTags.h"

UGA_HealingKit::UGA_HealingKit()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	ActivationBlockedTags.AddTag(DDGameplayTags::State_Character_Death);
}

void UGA_HealingKit::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                     FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthority(&ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ADDBasePlayerState* PS = Cast<ADDBasePlayerState>(ActorInfo->OwnerActor.Get());
	if (!PS)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(HealingEffect, 1.f);
	if (!SpecHandle.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag("Data.Health.Amount"),
		HealAmount
	);

	LOG_JJH(Warning, TEXT("Heal : %f"), HealAmount);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
