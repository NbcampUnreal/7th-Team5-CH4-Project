#include "BoardGame/Abilities/ItemAbilities/GA_GiveBomb.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "System/DDGameplayTags.h"

UGA_GiveBomb::UGA_GiveBomb()
{
	AbilityTags.AddTag(DDGameplayTags::Item_Ability_GiveBomb);
}

bool UGA_GiveBomb::ExecuteTargetingItem(AActor* TargetActor)
{
	UAbilitySystemComponent* SourceASC = GetBoardGameAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (!SourceASC || !TargetASC || !DamageEffect)
	{
		return false;
	}

	FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
	Context.AddSourceObject(GetAvatarActorFromActorInfo());
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffect, 1.f, Context);

	if (!SpecHandle.IsValid())
	{
		return false;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(DDGameplayTags::Data_Health_Damage, -Damage);
	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	return true;
}
