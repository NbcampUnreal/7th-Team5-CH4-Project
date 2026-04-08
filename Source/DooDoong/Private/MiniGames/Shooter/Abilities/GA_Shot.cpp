#include "MiniGames/Shooter/Abilities/GA_Shot.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AnimMontage.h"
#include "MiniGames/Shooter/Character/DDShooterCharacter.h"
#include "System/DDGameplayTags.h"

UGA_Shot::UGA_Shot()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	AbilityTags.AddTag(DDGameplayTags::Input_Ability_Shot);
	ActivationBlockedTags.AddTag(DDGameplayTags::State_Character_Death);
}

void UGA_Shot::ActivateAbility(
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

	if (ActorInfo == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CachedShooterCharacter = Cast<ADDShooterCharacter>(ActorInfo->AvatarActor.Get());
	if (CachedShooterCharacter == nullptr || !CachedShooterCharacter->BIsCanFire())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* MontageToPlay = FireMontage != nullptr ? FireMontage.Get() : CachedShooterCharacter->GetFireMontage();
	if (MontageToPlay == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_WaitGameplayEvent* WaitFireEventTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DDGameplayTags::Event_Shooter_Fire);
	WaitFireEventTask->EventReceived.AddDynamic(this, &ThisClass::OnReceiveFireEvent);
	WaitFireEventTask->ReadyForActivation();

	UAbilityTask_PlayMontageAndWait* PlayMontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay);
	PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCompleted);
	PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	PlayMontageTask->ReadyForActivation();
}

void UGA_Shot::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	CachedShooterCharacter = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Shot::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Shot::OnReceiveFireEvent(FGameplayEventData Payload)
{
	if (CachedShooterCharacter == nullptr || !CachedShooterCharacter->BIsCanFire())
	{
		return;
	}

	CachedShooterCharacter->HandleFireMontageNotify();
}
