#include "BoardGame/Abilities/DDRollDiceAbility.h"

#include "AbilitySystemComponent.h"
#include "BoardGame/DDTile.h"
#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/Abilities/DDMoveTileStepTask.h"
#include "Common/DDLogManager.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Base/Game/DDGameModeBase.h"
#include "System/DDGameplayTags.h"

UDDRollDiceAbility::UDDRollDiceAbility()
{
	// 한 번만 실행
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	ActivationRequiredTags.AddTag(DDGameplayTags::State_BoardGame_TurnActive);
    ActivationRequiredTags.AddTag(DDGameplayTags::State_TurnPhase_BeforeDice);
	
	ActivationBlockedTags.AddTag(DDGameplayTags::State_TurnPhase_Moving);
	ActivationBlockedTags.AddTag(DDGameplayTags::State_BoardGame_TurnWaiting);
}

void UDDRollDiceAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	LOG_CYS(Warning, TEXT("[GA_RD]Roll Dice"));
	
	if (ADDGameModeBase* GM = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode()))
    {
       GM->NotifyDiceRolled();
    }
	
	// 주사위
	DiceResult = FMath::RandRange(1, 6);
	ADDBoardGameCharacter* Character =
		Cast<ADDBoardGameCharacter>(GetAvatarActorFromActorInfo());

	if (!Character)
	{
		LOG_CYS(Error, TEXT("Character NULL"));

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	Character->PlayDice(DiceResult);

	// 연출 시간만 기다림
	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, 2.0f);
	DelayTask->OnFinish.AddDynamic(this, &UDDRollDiceAbility::OnDiceAnimationFinished);
	DelayTask->ReadyForActivation();
}

void UDDRollDiceAbility::OnDiceAnimationFinished()
{
	UDDMoveTileStepTask* Task = UDDMoveTileStepTask::MoveTile(this, DiceResult);
	if (!Task)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	Task->OnFinished.AddUniqueDynamic(this, &UDDRollDiceAbility::OnMoveFinished);
	Task->ReadyForActivation();
}

bool UDDRollDiceAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayTagContainer* SourceTags,
                                            const FGameplayTagContainer* TargetTags,
                                            FGameplayTagContainer* OptionalRelevantTags) const
{
	// if (!IsMyTurn) return false; // 턴 체크

	if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(
		DDGameplayTags::State_TurnPhase_Moving))
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UDDRollDiceAbility::OnMoveFinished()
{
	LOG_CYS(Warning, TEXT("[GA_RD] Move Finished"));

	if (ADDGameModeBase* GM = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode()))
    {
       GM->NotifyMovementFinished();
    }

	ADDBoardGameCharacter* Character =
		Cast<ADDBoardGameCharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ADDBasePlayerState* PS = Character->GetPlayerState<ADDBasePlayerState>();
	if (!PS)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ADDTile* CurrentTile = PS->CurrentTile;
	if (CurrentTile)
	{
		// 타일 Ability 트리거
		CurrentTile->OnCharacterArrived(Character);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UDDRollDiceAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
