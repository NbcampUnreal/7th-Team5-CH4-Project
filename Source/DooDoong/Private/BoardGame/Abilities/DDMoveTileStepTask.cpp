#include "BoardGame/Abilities/DDMoveTileStepTask.h"
#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/DDSelectableTileActor.h"
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
	if (!CurrentTile)
	{
		LOG_CYS(Error, TEXT("[MoveTask] NoCurrentTile"));
		EndTask();
		return;
	}

	// Goal(목표) 도착 시 이동 종료
	if (CurrentTile->IsGoal())
	{
		LOG_CYS(Warning, TEXT("[MoveTask] Goal"));
		OnFinished.Broadcast();
		EndTask();
		return;
	}

	// 다음 타일 선택 분기
	const TArray<ADDTile*>& NextTiles = CurrentTile->NextTiles;
	if (NextTiles.Num() == 0)
	{
		LOG_CYS(Error, TEXT("[MoveTask] NoNEXT"));
		EndTask();
	}
	else if (NextTiles.Num() == 1)
	{
		ContinueMove(NextTiles[0]);
	}
	else
	{
		// 분기
		SpawnSelectableActors(NextTiles);
		LOG_CYS(Error, TEXT("[MoveTask] 갈림길"));
	}
}

void UDDMoveTileStepTask::ContinueMove(ADDTile* Tile)
{
	ADDBoardGameCharacter* Character = Cast<ADDBoardGameCharacter>(GetAvatarActor());
	if (!Character) return;

	ADDBasePlayerState* PlayerState = Character->GetPlayerState<ADDBasePlayerState>();
	if (!PlayerState) return;

	LOG_CYS(Warning, TEXT("[MoveTask] NEXT: %s"), *Tile->TileRowName.ToString());

	ClearSelectableActors();

	// 위치 이동
	Character->MoveToLocation(Tile->GetStandLocation(Character));

	// 캐릭터 State 현재 타일 위치 업데이트
	PlayerState->CurrentTile = Tile;

	// 다음 스텝
	Character->OnMoveFinished.AddUniqueDynamic(this, &UDDMoveTileStepTask::OnCharacterMoveFinished);
}

void UDDMoveTileStepTask::SpawnSelectableActors(const TArray<ADDTile*>& Tiles)
{
	ADDBoardGameCharacter* Character = Cast<ADDBoardGameCharacter>(GetAvatarActor());
	if (!Character) return;

	for (ADDTile* Tile : Tiles)
	{
		FVector Loc = Tile->GetActorLocation();
		Loc.Z += 50.f;

		ADDSelectableTileActor* Actor =
			GetWorld()->SpawnActor<ADDSelectableTileActor>(Character->SelectableActorClass, Loc, FRotator::ZeroRotator);
		if (Actor)
		{
			Actor->TargetTile = Tile;
			Actor->SetOwner(Character);
			SpawnedActors.Add(Actor);
		}
	}

	// Character가 이 Task 참조하도록 (중요)
	Character->CurrentMoveTask = this;
}

void UDDMoveTileStepTask::SelectNextTile(ADDTile* SelectedTile)
{
	if (!SelectedTile) return;

	LOG_CYS(Warning, TEXT("[MoveTask] Selected: %s"), *SelectedTile->TileRowName.ToString());

	// 화살표 제거
	ClearSelectableActors();

	// 이동 이어서 진행
	ContinueMove(SelectedTile);
}

void UDDMoveTileStepTask::ClearSelectableActors()
{
	for (auto Actor : SpawnedActors)
	{
		if (Actor) Actor->Destroy();
	}
	SpawnedActors.Empty();
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
void UDDMoveTileStepTask::OnDestroy(bool AbilityEnded)
{
	ADDBoardGameCharacter* Character = Cast<ADDBoardGameCharacter>(GetAvatarActor());
	if (Character && Character->CurrentMoveTask == this)
	{
		Character->CurrentMoveTask = nullptr;
	}

	Super::OnDestroy(AbilityEnded);
}
