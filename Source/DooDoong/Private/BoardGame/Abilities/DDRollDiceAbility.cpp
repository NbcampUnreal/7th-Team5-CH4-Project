#include "BoardGame/Abilities/DDRollDiceAbility.h"

#include "AbilitySystemComponent.h"
#include "BoardGame/DDTile.h"
#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/Abilities/DDMoveTileStepTask.h"
#include "Common/DDLogManager.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "System/DDGameplayTags.h"

UDDRollDiceAbility::UDDRollDiceAbility()
{
	// 한 번만 실행
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	ActivationBlockedTags.AddTag(DDGameplayTags::State_TurnPhase_Moving);
}

void UDDRollDiceAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthority(&ActivationInfo))
	{
		return; // 서버 실행
	}
	if (bAlreadyRolled)
	{
		return;
	}

	LOG_CYS(Warning, TEXT("[GA_RD]Roll Dice"));
	// 주사위
	CachedDice = FMath::RandRange(1, 6);
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(DDGameplayTags::State_TurnPhase_Moving);
	bAlreadyRolled = true;
	ADDBoardGameCharacter* Character =
		Cast<ADDBoardGameCharacter>(GetAvatarActorFromActorInfo());

	if (!Character)
	{
		LOG_CYS(Error, TEXT("Character NULL"));
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(
			DDGameplayTags::State_TurnPhase_Moving
		);

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	Character->Multicast_PlayDiceAnimation(CachedDice);

	// 🎯 연출 시간만 기다림
	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, 2.0f);
	DelayTask->OnFinish.AddDynamic(this, &UDDRollDiceAbility::OnDiceAnimationFinished);
	DelayTask->ReadyForActivation();
}

void UDDRollDiceAbility::OnDiceAnimationFinished()
{
	UDDMoveTileStepTask* Task = UDDMoveTileStepTask::MoveTile(this, CachedDice);
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
	if (bAlreadyRolled)
	{
		return false;
	}
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
	// 타일 이벤트 실행
	// if (!HasAuthority(&CurrentActivationInfo))
	// {
	// 	return;
	// }

	ADDBoardGameCharacter* Character =
		Cast<ADDBoardGameCharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return;

	ADDBasePlayerState* PS = Character->GetPlayerState<ADDBasePlayerState>();
	if (!PS) return;

	ADDTile* CurrentTile = PS->CurrentTile;
	if (CurrentTile)
	{
		// 타일 Ability 트리거
		CurrentTile->OnCharacterArrived(Character);
	}
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(DDGameplayTags::State_TurnPhase_Moving);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
