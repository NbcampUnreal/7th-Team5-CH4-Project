#include "BoardGame/Abilities/ItemAbilities/ItemBase/GA_TargetingItemBase.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "EngineUtils.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "BoardGame/Game/DDBoardGameMode.h"
#include "System/DDGameplayTags.h"

UGA_TargetingItemBase::UGA_TargetingItemBase()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGA_TargetingItemBase::ActivateAbility(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* InitialTarget = TriggerEventData ? const_cast<AActor*>(TriggerEventData->Target.Get()) : nullptr;
	if (IsValid(InitialTarget))
	{
		bSelectingTarget = false;

		if (!CommitItemAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		const bool bExecuted = ExecuteTargetingItem(InitialTarget);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, !bExecuted);
		return;
	}

	if (!HasAuthority(&ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	bSelectingTarget = true;
	SelectedTargetIndex = INDEX_NONE;
	BuildTargetCandidates();

	if (CandidateTargets.IsEmpty())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	SelectedTargetIndex = 0;
	FocusSelectedTarget();

	UAbilityTask_WaitGameplayEvent* NextTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		DDGameplayTags::Event_Item_Target_Next,
		nullptr,
		false
	);
	if (NextTask)
	{
		NextTask->EventReceived.AddDynamic(this, &ThisClass::OnTargetNext);
		NextTask->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* PreviousTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		DDGameplayTags::Event_Item_Target_Previous,
		nullptr,
		false
	);
	if (PreviousTask)
	{
		PreviousTask->EventReceived.AddDynamic(this, &ThisClass::OnTargetPrevious);
		PreviousTask->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* ConfirmTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		DDGameplayTags::Event_Item_Target_Confirm
	);
	if (ConfirmTask)
	{
		ConfirmTask->EventReceived.AddDynamic(this, &ThisClass::OnTargetConfirm);
		ConfirmTask->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* CancelTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		DDGameplayTags::Event_Item_Target_Cancel
	);
	if (CancelTask)
	{
		CancelTask->EventReceived.AddDynamic(this, &ThisClass::OnTargetCancel);
		CancelTask->ReadyForActivation();
	}
}

void UGA_TargetingItemBase::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bSelectingTarget)
	{
		FocusCameraOnTarget(GetAvatarActorFromActorInfo());
	}

	CandidateTargets.Reset();
	SelectedTargetIndex = INDEX_NONE;
	bSelectingTarget = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_TargetingItemBase::OnTargetNext(FGameplayEventData Payload)
{
	ChangeTarget(1);
}

void UGA_TargetingItemBase::OnTargetPrevious(FGameplayEventData Payload)
{
	ChangeTarget(-1);
}

void UGA_TargetingItemBase::OnTargetConfirm(FGameplayEventData Payload)
{
	AActor* TargetActor = GetSelectedTarget();
	if (!IsValid(TargetActor))
	{
		OnTargetCancel(Payload);
		return;
	}

	if (bSelectingTarget)
	{
		const bool bCommitted = CommitItemAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
		if (!bCommitted)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}
	}

	const bool bExecuted = ExecuteTargetingItem(TargetActor);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, !bExecuted);
}

void UGA_TargetingItemBase::OnTargetCancel(FGameplayEventData Payload)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

bool UGA_TargetingItemBase::ExecuteTargetingItem(AActor* TargetActor)
{
	return false;
}

void UGA_TargetingItemBase::BuildTargetCandidates()
{
	CandidateTargets.Reset();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	AActor* OwnerActor = GetAvatarActorFromActorInfo();
	if (!OwnerActor)
	{
		return;
	}

	for (TActorIterator<ADDBoardGameCharacter> It(World); It; ++It)
	{
		ADDBoardGameCharacter* CandidateCharacter = *It;
		if (!IsValid(CandidateCharacter) || CandidateCharacter == OwnerActor)
		{
			continue;
		}

		CandidateTargets.Add(CandidateCharacter);
	}
}

void UGA_TargetingItemBase::ChangeTarget(int32 Offset)
{
	if (CandidateTargets.IsEmpty())
	{
		return;
	}

	SelectedTargetIndex = (SelectedTargetIndex + Offset + CandidateTargets.Num()) % CandidateTargets.Num();
	FocusSelectedTarget();
}

void UGA_TargetingItemBase::FocusSelectedTarget()
{
	FocusCameraOnTarget(GetSelectedTarget());
}

void UGA_TargetingItemBase::FocusCameraOnTarget(AActor* TargetActor)
{
	if (!IsValid(TargetActor))
	{
		return;
	}

	ADDBoardGameMode* BoardGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ADDBoardGameMode>() : nullptr;
	if (!BoardGameMode)
	{
		return;
	}

	BoardGameMode->FocusAllCamerasOnTarget(TargetActor);
}

AActor* UGA_TargetingItemBase::GetSelectedTarget() const
{
	return CandidateTargets.IsValidIndex(SelectedTargetIndex)
		       ? CandidateTargets[SelectedTargetIndex].Get()
		       : nullptr;
}
