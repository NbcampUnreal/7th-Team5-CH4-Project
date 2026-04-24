#include "BoardGame/Abilities/ItemAbilities/GA_Magnet.h"
#include "AbilitySystemComponent.h"
#include "Common/DDLogManager.h"

void UGA_Magnet::OnConfirm(FGameplayEventData Payload)
{
	Super::OnConfirm(Payload);
	
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass, 1.f);
	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetSetByCallerMagnitude(EffectDataTag, TotalDrainedAmount);
	
	// 코인 증가
	SourceASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	LOG_CYS(Warning, TEXT("UGA_Magnet: Successfully applied Magnet"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
