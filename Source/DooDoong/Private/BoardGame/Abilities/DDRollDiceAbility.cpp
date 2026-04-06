#include "BoardGame/Abilities/DDRollDiceAbility.h"
#include "BoardGame/DDTile.h"
#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/Abilities/DDMoveTileStepTask.h"
#include "Common/DDLogManager.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"

UDDRollDiceAbility::UDDRollDiceAbility()
{
	// 한 번만 실행
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
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
	LOG_CYS(Warning,TEXT("[GA_RD]Roll Dice"));
	// 주사위
	int32 Dice = FMath::RandRange(1, 6);

	// 이동 Task 실행
	UDDMoveTileStepTask* Task = UDDMoveTileStepTask::MoveTile(this, Dice);
	if (!Task)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}


	Task->OnFinished.AddUniqueDynamic(this, &UDDRollDiceAbility::OnMoveFinished);

	Task->ReadyForActivation();
	
}

bool UDDRollDiceAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// if (!IsMyTurn) return false; // 턴 체크
	if (bAlreadyRolled) return false;
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
		CurrentTile->TriggerTileAbility(Character);
	}
	// 중복 실행 방지
	bAlreadyRolled=true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
