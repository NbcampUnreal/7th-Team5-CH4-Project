#include "MiniGames/Shooter/Abilities/GA_Shot.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AnimMontage.h"
#include "Common/DDLogManager.h"
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

	LOG_JJH(Log, TEXT("[GA_Shot] 어빌리티 활성화"));

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		LOG_JJH(Warning, TEXT("[GA_Shot] 어빌리티 커밋 실패"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ActorInfo == nullptr)
	{
		LOG_JJH(Warning, TEXT("[GA_Shot] 액터가 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CachedShooterCharacter = Cast<ADDShooterCharacter>(ActorInfo->AvatarActor.Get());
	if (CachedShooterCharacter == nullptr || !CachedShooterCharacter->BIsCanFire())
	{
		LOG_JJH(Warning, TEXT("[GA_Shot] 슈터 캐릭터가 없거나 발사할 수 없는 상태"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* MontageToPlay = FireMontage != nullptr ? FireMontage.Get() : CachedShooterCharacter->GetFireMontage();
	if (MontageToPlay == nullptr)
	{
		LOG_JJH(Warning, TEXT("[GA_Shot] 몽타주가 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	LOG_JJH(Log, TEXT("[GA_Shot] Play montage: %s"), *MontageToPlay->GetName());

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
	LOG_JJH(Log, TEXT("[GA_Shot] 발사 이벤트 트리거"));

	if (CachedShooterCharacter == nullptr || !CachedShooterCharacter->BIsCanFire())
	{
		LOG_JJH(Warning, TEXT("[GA_Shot] 캐릭터가 없거나 발사할 수 없어서 이벤트 실패"));
		return;
	}

	CachedShooterCharacter->HandleFireMontageNotify();
}
