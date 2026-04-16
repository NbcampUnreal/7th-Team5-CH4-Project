#include "BoardGame/Abilities/ItemAbilities/GA_Targeting.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "EngineUtils.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "BoardGame/Character/Components/ItemActionComponent.h"
#include "System/DDGameplayTags.h"

UGA_Targeting::UGA_Targeting()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	AbilityTags.AddTag(DDGameplayTags::ItemAction_Ability_Targeting);
	ActivationBlockedTags.AddTag(DDGameplayTags::State_Character_Death);
}

void UGA_Targeting::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* AvatarActor = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;
	ItemActionComponent = AvatarActor ? AvatarActor->FindComponentByClass<UItemActionComponent>() : nullptr;

	SelectedTargetIndex = INDEX_NONE;
	BuildTargetCandidates();

	if (CandidateTargets.IsEmpty())
	{
		SendTargetResultEvent(DDGameplayTags::Event_Item_TargetCanceled, nullptr);
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

void UGA_Targeting::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ItemActionComponent = nullptr;
	CandidateTargets.Reset();
	SelectedTargetIndex = INDEX_NONE;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Targeting::OnTargetNext(FGameplayEventData Payload)
{
	ChangeTarget(1);
}

void UGA_Targeting::OnTargetPrevious(FGameplayEventData Payload)
{
	ChangeTarget(-1);
}

void UGA_Targeting::OnTargetConfirm(FGameplayEventData Payload)
{
	AActor* TargetActor = GetSelectedTarget();
	if (!IsValid(TargetActor))
	{
		OnTargetCancel(Payload);
		return;
	}

	SendTargetResultEvent(DDGameplayTags::Event_Item_TargetConfirmed, TargetActor);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Targeting::OnTargetCancel(FGameplayEventData Payload)
{
	SendTargetResultEvent(DDGameplayTags::Event_Item_TargetCanceled, nullptr);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Targeting::BuildTargetCandidates()
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

void UGA_Targeting::ChangeTarget(int32 Offset)
{
	if (CandidateTargets.IsEmpty())
	{
		return;
	}

	SelectedTargetIndex = (SelectedTargetIndex + Offset + CandidateTargets.Num()) % CandidateTargets.Num();
	FocusSelectedTarget();
}

void UGA_Targeting::FocusSelectedTarget()
{
	if (ItemActionComponent)
	{
		ItemActionComponent->Server_FocusItemTarget(GetSelectedTarget());
	}
}

void UGA_Targeting::SendTargetResultEvent(const FGameplayTag& EventTag, AActor* TargetActor)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor) || !EventTag.IsValid())
	{
		return;
	}

	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.Instigator = AvatarActor;
	Payload.Target = TargetActor;
	Payload.OptionalObject = ItemActionComponent;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(AvatarActor, EventTag, Payload);
}

AActor* UGA_Targeting::GetSelectedTarget() const
{
	return CandidateTargets.IsValidIndex(SelectedTargetIndex)
		       ? CandidateTargets[SelectedTargetIndex].Get()
		       : nullptr;
}
