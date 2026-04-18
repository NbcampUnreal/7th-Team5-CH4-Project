//GA_Throw.cpp
#include "MiniGames/Ricochet/Abilities/GA_Throw.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AnimMontage.h"
#include "Common/DDLogManager.h"
#include "MiniGames/Ricochet/Character/DDRicochetCharacter.h"
#include "System/DDGameplayTags.h"

UGA_Throw::UGA_Throw()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	AbilityTags.AddTag(DDGameplayTags::Input_Ability_Throw);
	ActivationBlockedTags.AddTag(DDGameplayTags::State_Character_Death);
}

void UGA_Throw::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	LOG_JJH(Log, TEXT("[GA_Throw] 어빌리티 활성화"));

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		LOG_JJH(Warning, TEXT("[GA_Throw] 커밋 실패"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ActorInfo == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CachedCharacter = Cast<ADDRicochetCharacter>(ActorInfo->AvatarActor.Get());
	if (CachedCharacter == nullptr || !CachedCharacter->BIsCanThrow())
	{
		LOG_JJH(Warning, TEXT("[GA_Throw] 던질 수 없는 상태"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* MontageToPlay =
		ThrowMontage != nullptr ? ThrowMontage.Get() : CachedCharacter->GetThrowMontage();

	if (MontageToPlay == nullptr)
	{
		LOG_JJH(Warning, TEXT("[GA_Throw] 몽타주 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_WaitGameplayEvent* WaitTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DDGameplayTags::Event_Ricochet_Throw);
	WaitTask->EventReceived.AddDynamic(this, &ThisClass::OnReceiveThrowEvent);
	WaitTask->ReadyForActivation();

	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCompleted);
	MontageTask->ReadyForActivation();
}

void UGA_Throw::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	CachedCharacter = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Throw::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Throw::OnReceiveThrowEvent(FGameplayEventData Payload)
{
	LOG_JJH(Log, TEXT("[GA_Throw] 던지기 이벤트"));

	if (CachedCharacter == nullptr || !CachedCharacter->BIsCanThrow())
	{
		LOG_JJH(Warning, TEXT("[GA_Throw] 캐릭터 상태 문제"));
		return;
	}

	CachedCharacter->HandleThrowMontageNotify();
}