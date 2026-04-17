#include "BoardGame/Abilities/ItemAbilities/ItemBase/GA_ItemBase.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
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

bool UGA_ItemBase::PlayItemMontage(float PlayRate, FName StartSection)
{
	if (!ItemMontage)
	{
		return false;
	}

	ActiveItemMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		ItemMontage,
		PlayRate,
		StartSection
	);

	if (!ActiveItemMontageTask)
	{
		return false;
	}

	ActiveItemMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnItemMontageCompleted);
	ActiveItemMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnItemMontageInterrupted);
	ActiveItemMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnItemMontageCancelled);
	ActiveItemMontageTask->ReadyForActivation();
	return true;
}

void UGA_ItemBase::OnItemMontageCompleted()
{
	ActiveItemMontageTask = nullptr;
}

void UGA_ItemBase::OnItemMontageInterrupted()
{
	ActiveItemMontageTask = nullptr;
}

void UGA_ItemBase::OnItemMontageCancelled()
{
	ActiveItemMontageTask = nullptr;
}
