#include "Base/MiniGame/DDMiniGameStateBase.h"

#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

ADDMiniGameStateBase::ADDMiniGameStateBase()
{
	bReplicates = true;
}

void ADDMiniGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 진행 상태와 점수판은 모든 클라이언트가 동일하게 봐야하기 때문에 복제
	DOREPLIFETIME(ADDMiniGameStateBase, CurrentState);
	DOREPLIFETIME(ADDMiniGameStateBase, RemainingTimeSeconds);
	DOREPLIFETIME(ADDMiniGameStateBase, Participants);
	DOREPLIFETIME(ADDMiniGameStateBase, ScoreBoard);
}

void ADDMiniGameStateBase::SetMiniGameState(FGameplayTag NewState)
{
	// 현재 진행 단계를 갱신할 때 사용
	CurrentState = NewState;
}

void ADDMiniGameStateBase::SetRemainingTimeSeconds(float NewRemainingTimeSeconds)
{
	// 게임의 남은 시간을 갱신 및 저장 (UI에서 사용하거나 할 수 있도록)
	RemainingTimeSeconds = NewRemainingTimeSeconds;
}

void ADDMiniGameStateBase::SetParticipants(const TArray<FMiniGameParticipantInfo>& InParticipants)
{
	// 현재 미니게임에 참여 중인 플레이어 목록을 갱신
	Participants = InParticipants;
}

void ADDMiniGameStateBase::SetScoreBoard(const TArray<FMiniGameScoreEntry>& InScoreBoard)
{
	// 룰셋에서 정리한 점수판을 통째로 반영할 때 사용할 수 있음
	ScoreBoard = InScoreBoard;
}

void ADDMiniGameStateBase::AddScore(APlayerState* PlayerState, int32 DeltaScore)
{
	if (PlayerState == nullptr)
	{
		return;
	}

	// 플레이어의 기존 점수 기록이 이미 ScoreBoard에 있는지 확인하는 작업.
	FMiniGameScoreEntry* ExistingEntry = ScoreBoard.FindByPredicate([PlayerState](const FMiniGameScoreEntry& Entry)
	{
		return Entry.PlayerState == PlayerState;
	});

	// 만약 스코어 보드에 없다면, 즉 플레이어가 처음으로 득점했다면 점수판 엔트리를 새로 만들어서 추가
	if (ExistingEntry == nullptr)
	{
		FMiniGameScoreEntry NewEntry;
		NewEntry.PlayerState = PlayerState;
		NewEntry.PlayerName = PlayerState->GetPlayerName();
		NewEntry.Score = DeltaScore;
		ScoreBoard.Add(NewEntry);
		return;
	}
	
	// 이미 점수를 획득했던 플레이어는 해당하는 점수판에 점수를 계속 누적하는 방식
	ExistingEntry->PlayerName = PlayerState->GetPlayerName();
	ExistingEntry->Score += DeltaScore;
}

int32 ADDMiniGameStateBase::GetScore(APlayerState* PlayerState) const
{
	if (PlayerState == nullptr)
	{
		return 0;
	}
	
	// 플레이어의 기존 점수 기록이 이미 ScoreBoard에 있는지 확인하는 작업.
	const FMiniGameScoreEntry* ExistingEntry = ScoreBoard.FindByPredicate([PlayerState](const FMiniGameScoreEntry& Entry)
	{
		return Entry.PlayerState == PlayerState;
	});

	// 플레이어의 기록이 있다면 return, 없다면 0을 return
	return ExistingEntry != nullptr ? ExistingEntry->Score : 0;
}
