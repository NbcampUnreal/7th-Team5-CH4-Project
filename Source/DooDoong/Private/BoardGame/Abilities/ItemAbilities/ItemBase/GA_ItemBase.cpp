#include "BoardGame/Abilities/ItemAbilities/ItemBase/GA_ItemBase.h"

#include "AbilitySystemComponent.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "System/DDGameplayTags.h"

UGA_ItemBase::UGA_ItemBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = DDGameplayTags::Event_Item_Activate;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	ActivationBlockedTags.AddTag(DDGameplayTags::State_Character_Death);
}

void UGA_ItemBase::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

ADDBoardGameCharacter* UGA_ItemBase::GetBoardGameCharacter() const
{
	return Cast<ADDBoardGameCharacter>(GetAvatarActorFromActorInfo());
}

UAbilitySystemComponent* UGA_ItemBase::GetBoardGameAbilitySystemComponent() const
{
	const ADDBoardGameCharacter* BoardGameCharacter = GetBoardGameCharacter();
	return BoardGameCharacter ? BoardGameCharacter->GetAbilitySystemComponent() : nullptr;
}
