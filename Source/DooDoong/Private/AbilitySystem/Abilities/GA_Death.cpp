#include "AbilitySystem/Abilities/GA_Death.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "System/DDGameplayTags.h"

UGA_Death::UGA_Death()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bRetriggerInstancedAbility = false; 
		
	AbilityTags.AddTag(DDGameplayTags::State_Character_Death); 
}

void UGA_Death::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return; 
	
	GetAbilitySystemComponentFromActorInfo()->CancelAllAbilities(); 
		
	UAbilityTask_PlayMontageAndWait* MontageTask = 
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, 
			TEXT("Death"),
			DeathMontage
		);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Death::OnMontageFinished);
	MontageTask->ReadyForActivation();

	if (Character->HasAuthority())
	{
		Character->GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		Character->GetCharacterMovement()->StopMovementImmediately();
		Character->GetCharacterMovement()->DisableMovement();
		
		Character->SetLifeSpan(5.f); 
	}
}

void UGA_Death::OnMontageFinished()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	Character->GetMesh()->SetSimulatePhysics(true);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false); 
}
