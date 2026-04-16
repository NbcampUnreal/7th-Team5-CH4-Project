#include "BoardGame/Abilities/ItemAbilities/GA_HealingKit.h"

#include "AbilitySystemComponent.h"
#include "Common/DDLogManager.h"
#include "System/DDGameplayTags.h"

UGA_HealingKit::UGA_HealingKit()
{
	AbilityTags.AddTag(DDGameplayTags::Item_Ability_HealingKit);
}

bool UGA_HealingKit::ExecuteInstantItem(const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = GetBoardGameAbilitySystemComponent();
	if (!ASC)
	{
		return false;
	}

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(HealingEffect, 1.f);
	if (!SpecHandle.IsValid())
	{
		return false;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(
		DDGameplayTags::Data_Health_Heal,
		HealAmount
	);

	LOG_JJH(Warning, TEXT("Heal : %f"), HealAmount);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	return true;
}
