#include "BoardGame/Abilities/DDMoveTileStepTask.h"

#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/DDTile.h"
#include "Common/DDLogManager.h"

UDDMoveTileStepTask* UDDMoveTileStepTask::MoveTile(UGameplayAbility* OwningAbility, int32 Steps)
{
	LOG_CYS(Warning, TEXT("[MoveTask] MOVE: %s"), *OwningAbility->GetName());
	LOG_CYS(Warning, TEXT("[MoveTask] Steps: %d"), Steps);

	auto Task = NewAbilityTask<UDDMoveTileStepTask>(OwningAbility);
	Task->RemainingSteps = Steps;
	return Task;
}

void UDDMoveTileStepTask::Activate()
{
	LOG_CYS(Warning, TEXT("[MoveTask] Activate"));
	MoveNext();
}

void UDDMoveTileStepTask::MoveNext()
{
	LOG_CYS(Warning, TEXT("[MoveTask] MoveNext"));

	if (RemainingSteps <= 0)
	{
		// 이동완료, 태스크 종료
		OnFinished.Broadcast();
		EndTask();
		return;
	}

	ADDBoardGameCharacter* Character = Cast<ADDBoardGameCharacter>(GetAvatarActor());
	if (!Character)
	{
		LOG_CYS(Error, TEXT("[MoveTask] NoCharacter"));
		return;
	}

	// 캐릭터 State 에서 현재 타일 위치 가져오기
	ADDBasePlayerState* PlayerState = Character->GetPlayerState<ADDBasePlayerState>();

	if (!PlayerState) return;

	ADDTile* CurrentTile = PlayerState->CurrentTile;

	// Goal(목표) 도착 시 이동 종료
	if (CurrentTile->IsGoal())
	{
		LOG_CYS(Warning, TEXT("[MoveTask] Goal"));
		OnFinished.Broadcast();
		EndTask();
		return;
	}

	if (!CurrentTile)
	{
		LOG_CYS(Error, TEXT("[MoveTask] NoCurrentTile"));

		EndTask();
		return;
	}
	// 다음 타일 선택 (일단 랜덤)
	if (CurrentTile->NextTiles.Num() == 0)
	{
		LOG_CYS(Error, TEXT("[MoveTask] NoNEXT"));

		return;
	}

	ADDTile* NextTile = CurrentTile->NextTiles[0]; // TODO: 분기 선택

	LOG_CYS(Warning, TEXT("[MoveTask] NEXT: %s"), *NextTile->TileRowName.ToString());

	// 위치 이동
	Character->MoveToLocation(NextTile->GetStandLocation(Character));

	// 캐릭터 State 현재 타일 위치 업데이트
	PlayerState->CurrentTile = NextTile;

	// 다음 스텝
	Character->OnMoveFinished.AddUniqueDynamic(this, &UDDMoveTileStepTask::OnCharacterMoveFinished);
}

void UDDMoveTileStepTask::OnCharacterMoveFinished()
{
	ADDBoardGameCharacter* Character = Cast<ADDBoardGameCharacter>(GetAvatarActor());
	if (!Character) return;

	// Delegate 제거 (중복 방지)
	Character->OnMoveFinished.RemoveDynamic(this, &UDDMoveTileStepTask::OnCharacterMoveFinished);

	RemainingSteps--;

	MoveNext();
}
