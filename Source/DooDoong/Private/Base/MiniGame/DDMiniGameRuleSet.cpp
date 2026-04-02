#include "Base/MiniGame/DDMiniGameRuleSet.h"

#include "Base/MiniGame/DDMiniGameStateBase.h"

void UDDMiniGameRuleSet::InitializeRules(const FMiniGameSetup& InSetup)
{
	// RuleSet이 공통으로 참조할 미니게임 설정을 캐싱
	CachedSetup = InSetup;
}

bool UDDMiniGameRuleSet::ShouldFinishGame(const ADDMiniGameStateBase* GameState) const
{
	// 고유의 종료 조건 : 기본적으로는 false
	return false;
}

void UDDMiniGameRuleSet::AddScore(ADDMiniGameStateBase* GameState, APlayerState* PlayerState, int32 DeltaScore)
{
	if (GameState == nullptr || PlayerState == nullptr)
	{
		return;
	}

	// 기본 구현은 단순 더하기로 점수를 기록하는 것으로 해야해서 GameState의 로직을 그대로 씀. 
	// 개별 미니게임에서 오버라이드 해서 새로 로직을 작성하는 것으로
	GameState->AddScore(PlayerState, DeltaScore);
}

void UDDMiniGameRuleSet::ResolveRanking(ADDMiniGameStateBase* GameState) const
{
	if (GameState == nullptr)
	{
		return;
	}

	TArray<FMiniGameScoreEntry> SortedScoreBoard = GameState->GetScoreBoard();
	// 점수 내림차순으로 정렬해 최종 순위를 확정
	SortedScoreBoard.Sort([](const FMiniGameScoreEntry& Left, const FMiniGameScoreEntry& Right)
	{
		return Left.Score > Right.Score;
	});

	for (int32 Index = 0; Index < SortedScoreBoard.Num(); ++Index)
	{
		SortedScoreBoard[Index].Rank = Index + 1;
	}

	// GameState에 반영
	GameState->SetScoreBoard(SortedScoreBoard);
}

FMiniGameResult UDDMiniGameRuleSet::BuildMiniGameResult(const ADDMiniGameStateBase* GameState) const
{
	// 결과값 구조체
	FMiniGameResult Result;
	Result.MiniGameId = CachedSetup.MiniGameID;
	
	if (GameState == nullptr)
	{
		return Result;
	}
	// ResolveRanking보다 무조건 이후에 호출되기 때문에 GameState에 모인 점수판을 결과 구조체로 옮겨두고 그대로 사용
	Result.ScoreBoard = GameState->GetScoreBoard();
	for (const FMiniGameScoreEntry& Entry : Result.ScoreBoard)
	{
		Result.Ranking.Add(Entry.PlayerState);
	}

	return Result;
}
