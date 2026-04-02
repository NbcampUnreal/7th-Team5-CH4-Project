#include "BoardGame/Abilities/DDMoveTileStepTask.h"
#include "BoardGame/DDTile.h"
#include "Common/DDLogManager.h"

UDDMoveTileStepTask* UDDMoveTileStepTask::MoveTile(UGameplayAbility* OwningAbility, int32 Steps)
{
	LOG_CYS(Warning,TEXT("[MT]MOVE:%s"),*OwningAbility->GetName());
	LOG_CYS(Warning,TEXT("[MT]Steps:%d"),Steps);
	
	auto Task = NewAbilityTask<UDDMoveTileStepTask>(OwningAbility);
	Task->RemainingSteps = Steps;
	return Task;
}

void UDDMoveTileStepTask::Activate()
{
	LOG_CYS(Warning,TEXT("[MT]Activate"));
	MoveNext();
}

void UDDMoveTileStepTask::MoveNext()
{
	LOG_CYS(Warning,TEXT("[MT]MoveNext"));
	
	if (RemainingSteps <= 0)
	{
		OnFinished.Broadcast();
		EndTask();
		return;
	}

	ADDBoardGameCharacter* Character = Cast<ADDBoardGameCharacter>(GetAvatarActor());
	if (!Character)
	{
		LOG_CYS(Warning,TEXT("[MT]NoCharacter"));
		return;
	}

	// TODO: 캐릭터 State 에서 현재 타일 위치 가져오기
	
	// Dump: 에디터에서 현재 타일 임시 지정
	ADDTile* CurrentTile = Character->StartTile;
	if (!CurrentTile)
	{
	LOG_CYS(Warning,TEXT("[MT]NoCurrentTile"));
		
		EndTask();
		return;
	}
	// 다음 타일 선택 (일단 랜덤)
	if (CurrentTile->NextTiles.Num() == 0)
	{
		LOG_CYS(Warning,TEXT("[MT]NoNEXT"));
		
		return;
	}

	ADDTile* NextTile = CurrentTile->NextTiles[0]; // TODO: 분기 선택

	LOG_CYS(Warning,TEXT("[MT]NEXT:%s"),*NextTile->TileRowName.ToString());
	
	// 위치 이동 (텔레포트 → 나중에 보간 가능)
	// Character->SetActorLocation(NextTile->GetStandLocation());
	Character->MoveToLocation(NextTile->GetStandLocation(Character));
	
	// TODO: 캐릭터 State 현재 타일 위치 업데이트
	// BoardComp->SetCurrentTile(NextTile);
	Character->StartTile=NextTile;

	// 다음 스텝
	// GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UDDMoveTileStepTask::MoveNext);
	Character->OnMoveFinished.AddUniqueDynamic(this,&UDDMoveTileStepTask::OnCharacterMoveFinished);
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