#include "BoardGame/Abilities/ItemAbilities/ItemBase/GA_ItemBase.h"

#include "AbilitySystemBlueprintLibrary.h"
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

bool UGA_ItemBase::ExecuteItemCue(const FGameplayTag& CueTag) const
{
	if (!CueTag.IsValid())
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystemComponent = GetBoardGameAbilitySystemComponent();
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AbilitySystemComponent || !AvatarActor)
	{
		return false;
	}

	FGameplayCueParameters CueParameters;
	CueParameters.Instigator = AvatarActor;
	CueParameters.EffectCauser = AvatarActor;
	CueParameters.Location = AvatarActor->GetActorLocation();

	AbilitySystemComponent->ExecuteGameplayCue(CueTag, CueParameters);
	return true;
}

bool UGA_ItemBase::ExecuteItemCueOnTarget(const FGameplayTag& CueTag, AActor* CueActor) const
{
	if (!CueTag.IsValid() || !IsValid(CueActor))
	{
		return false;
	}

	UAbilitySystemComponent* TargetAbilitySystemComponent =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CueActor);
	if (!TargetAbilitySystemComponent)
	{
		return false;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	FGameplayCueParameters CueParameters;
	CueParameters.Instigator = CueActor;
	CueParameters.EffectCauser = AvatarActor ? AvatarActor : CueActor;
	CueParameters.Location = CueActor->GetActorLocation();

	TargetAbilitySystemComponent->ExecuteGameplayCue(CueTag, CueParameters);
	return true;
}
