#include "BoardGame/Abilities/DDMoveTileStep.h"
#include "BoardGame/DDTile.h"
#include "Base/Character/DDBaseCharacter.h"
#include "Common/DDLogManager.h"
UDDMoveTileStep* UDDMoveTileStep::MoveTile(UGameplayAbility* OwningAbility, int32 Steps)
{
	LOG_CYS(Warning,TEXT("MOVE:%s"),*OwningAbility->GetName());
	LOG_CYS(Warning,TEXT("Steps:%d"),Steps);
	
	auto Task = NewAbilityTask<UDDMoveTileStep>(OwningAbility);
	Task->RemainingSteps = Steps;
	return Task;
}

void UDDMoveTileStep::Activate()
{
	LOG_CYS(Warning,TEXT("Activate"));
	MoveNext();
}

void UDDMoveTileStep::MoveNext()
{
	LOG_CYS(Warning,TEXT("MoveNext"));
	
	if (RemainingSteps <= 0)
	{
		OnFinished.Broadcast();
		EndTask();
		return;
	}

	ADDBaseCharacter* Character = Cast<ADDBaseCharacter>(GetAvatarActor());
	if (!Character)
	{
		LOG_CYS(Warning,TEXT("NoCharactor"));
		return;
	}

	// TODO: 캐릭터 State 에서 현재 타일 위치 가져오기
	// UBoardStateComponent* BoardComp = Character->FindComponentByClass<UBoardStateComponent>();
	// if (!BoardComp) return;

	// ATile* CurrentTile = BoardComp->GetCurrentTile();
	// if (!CurrentTile) return;
	
	// Dump: 에디터에서 현재 타일 임시 지정
	ADDTile* CurrentTile = Character->StartTile;
	if (!CurrentTile)
	{
	LOG_CYS(Warning,TEXT("NoCurrentTile"));
		
		EndTask();
		return;
	}
	// 다음 타일 선택 (일단 랜덤)
	if (CurrentTile->NextTiles.Num() == 0)
	{
	LOG_CYS(Warning,TEXT("NoNEXT"));
		
		return;
	}

	ADDTile* NextTile = CurrentTile->NextTiles[0]; // TODO: 분기 선택

	LOG_CYS(Warning,TEXT("NEXT:%s"),*NextTile->TileRowName.ToString());
	// 위치 이동 (텔레포트 → 나중에 보간 가능)
	Character->SetActorLocation(NextTile->GetStandLocation());

	// TODO: 캐릭터 State 현재 타일 위치 업데이트
	// BoardComp->SetCurrentTile(NextTile);

	RemainingSteps--;

	// 다음 스텝
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UDDMoveTileStep::MoveNext);
}
