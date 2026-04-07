#include "AbilitySystem/Abilities/GA_Death.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Base/Character/DDBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "System/DDGameplayTags.h"

UGA_Death::UGA_Death()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bRetriggerInstancedAbility = false; 
	
	ActivationBlockedTags.AddTag(DDGameplayTags::State_Character_Death); 
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = DDGameplayTags::Event_Character_Death;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_Death::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UE_LOG(LogTemp,Warning, TEXT("UGA_Death ActivateAbility ")); 
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	ADDBaseCharacter* Character = Cast<ADDBaseCharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return; 
	
	GetAbilitySystemComponentFromActorInfo()->CancelAllAbilities(); 
	
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(DDGameplayTags::State_Character_Death);
		
	UAbilityTask_PlayMontageAndWait* MontageTask = 
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, 
			TEXT("Death"),
			DeathMontage
		);
	
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Death::OnMontageFinished);
	MontageTask->ReadyForActivation();
}

void UGA_Death::OnMontageFinished()
{
	ADDBaseCharacter* Character = Cast<ADDBaseCharacter>(GetAvatarActorFromActorInfo());
	if (Character) Character->MultiCast_CharacterDeath(); 
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false); 
}
