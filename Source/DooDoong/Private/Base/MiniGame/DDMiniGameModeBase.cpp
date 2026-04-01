#include "Base/MiniGame/DDMiniGameModeBase.h"

#include "Base/MiniGame/DDMiniGameRuleSet.h"
#include "Base/MiniGame/DDMiniGameStateBase.h"
#include "System/MiniGame/DDMiniGameDefinition.h"
#include "System/MiniGame/DDMiniGameManager.h"
#include "TimerManager.h"

ADDMiniGameModeBase::ADDMiniGameModeBase()
{
	GameStateClass = ADDMiniGameStateBase::StaticClass();
	PrimaryActorTick.bCanEverTick = false;
}

void ADDMiniGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	// GameMode는 서버에만 존재하므로 서버가 아닌 경우 바로 종료
	if (!HasAuthority() || GetGameInstance() == nullptr)
	{
		return;
	}
	
	UDDMiniGameManager* MiniGameManager = GetGameInstance()->GetSubsystem<UDDMiniGameManager>();
	if (MiniGameManager == nullptr || !MiniGameManager->HasActiveMiniGame() || !MiniGameManager->IsActiveMiniGameWorld(GetWorld()))
	{
		return;
	}
	
	// 미니게임 세팅 Initialize
	InitializeMiniGame(MiniGameManager->GetActiveSetup(), MiniGameManager->GetActiveParticipants());
	// RuleSet이 있다면 Initialize
	InitializeRuleSet();
	// 미니게임 시작
	StartMiniGame();
	
	MiniGameManager->NotifyMiniGameStarted();
}

void ADDMiniGameModeBase::UpdateMiniGameTime()
{
	if (!HasAuthority() || !bIsMiniGameStarted || bIsMiniGameFinished)
	{
		return;
	}
	
	// 경과한 시간을 타이머 갱신 주기만큼 누적
	ElapsedTimeSeconds += TimeUpdateIntervalSeconds;
	
	if (ADDMiniGameStateBase* MiniGameState = GetMiniGameState())
	{
		// 남은 시간을 (게임 제한시간 - 경과한 시간)으로 계산하고 GameState 모든 클라이언트와 동기화
		const float RemainingTimeSeconds = ActiveSetup.TimeLimitSeconds > 0.f
			? FMath::Max(0.f, ActiveSetup.TimeLimitSeconds - ElapsedTimeSeconds)
			: 0.f;
		
		MiniGameState->SetRemainingTimeSeconds(RemainingTimeSeconds);
	}
	
	if (RuleSet != nullptr && RuleSet->ShouldFinishGame(GetMiniGameState()))
	{
		// 개별 룰셋이 종료 조건을 만족했다고 판단하면 남은 시간과 관계없이 즉시 종료
		FinishGame(DDMiniGameplayTags::FinishReason_Completed);
		return;
	}
	
	// 제한 시간이 끝나면 게임을 TimeOver 태그에 의해 종료
	if (ActiveSetup.TimeLimitSeconds > 0.f && ElapsedTimeSeconds >= ActiveSetup.TimeLimitSeconds)
	{
		FinishGame(DDMiniGameplayTags::FinishReason_TimeOver);
	}
}

void ADDMiniGameModeBase::InitializeMiniGame(const FMiniGameSetup& InSetup,
	const TArray<FMiniGameParticipantInfo>& InParticipants)
{
	ActiveSetup = InSetup;
	ActiveParticipants = InParticipants;
	bIsMiniGameStarted = false;
	bIsMiniGameFinished = false;
	ElapsedTimeSeconds = 0.f;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MiniGameTimerHandle);
	}
	
	if (ADDMiniGameStateBase* MiniGameState = GetMiniGameState())
	{
		// 클라이언트가 확인할 초기 상태는 GameState에 복제 가능한 형태로 저장
		MiniGameState->SetParticipants(ActiveParticipants);
		MiniGameState->SetRemainingTimeSeconds(ActiveSetup.TimeLimitSeconds);
		MiniGameState->SetMiniGameState(DDMiniGameplayTags::State_Preparing);
		MiniGameState->SetScoreBoard(TArray<FMiniGameScoreEntry>());
	}
}

void ADDMiniGameModeBase::StartMiniGame()
{
	// 플레이 시작 및 시작 시점을 기록
	bIsMiniGameStarted = true;
	ElapsedTimeSeconds = 0.0f;
	
	// GameState에서 시작 상태 태그를 갱신 & 게임의 남은 시간을 게임 데이터의 제한시간으로 갱신
	if (ADDMiniGameStateBase* MiniGameState = GetMiniGameState())
	{
		MiniGameState->SetMiniGameState(DDMiniGameplayTags::State_Playing);
		MiniGameState->SetRemainingTimeSeconds(ActiveSetup.TimeLimitSeconds);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(MiniGameTimerHandle, this, &ADDMiniGameModeBase::UpdateMiniGameTime, TimeUpdateIntervalSeconds, true);
	}
}

void ADDMiniGameModeBase::FinishMiniGame()
{
	// 기본적으로 사용하는 종료 요청
	FinishGame(DDMiniGameplayTags::FinishReason_Completed);
}

void ADDMiniGameModeBase::StopMiniGame(FGameplayTag Reason)
{
	// 게임 종료의 원인을 태그로 받아 미니게임을 강제로 종료
	FinishGame(Reason);
}

FMiniGameResult ADDMiniGameModeBase::BuildMiniGameResult() const
{
	// 룰셋이 있으면 룰셋 결과를 우선 사용하고, 없으면 기본 결과를 생성
	FMiniGameResult Result = RuleSet != nullptr
		? RuleSet->BuildMiniGameResult(GetMiniGameState())
		: FMiniGameResult();

	// 
	Result.MiniGameId = ActiveSetup.MiniGameID;
	if (const ADDMiniGameStateBase* MiniGameState = GetMiniGameState())
	{
		Result.ScoreBoard = MiniGameState->GetScoreBoard();
		if (Result.Ranking.Num() == 0)
		{
			for (const FMiniGameScoreEntry& Entry : Result.ScoreBoard)
			{
				Result.Ranking.Add(Entry.PlayerState);
			}
		}
	}

	return Result;
}

void ADDMiniGameModeBase::AddScore(APlayerState* PlayerState, int32 DeltaScore)
{
	ADDMiniGameStateBase* MiniGameState = GetMiniGameState();
	if (MiniGameState == nullptr || PlayerState == nullptr)
	{
		return;
	}
	
	// RuleSet이 있다면 RuleSet의 점수 계산 규칙을 우선 적용
	if (RuleSet != nullptr)
	{
		RuleSet->AddScore(MiniGameState, PlayerState, DeltaScore);
	}
	else
	{
		MiniGameState->AddScore(PlayerState, DeltaScore);
	}
}

void ADDMiniGameModeBase::FinishGame(FGameplayTag Reason)
{
	if (bIsMiniGameFinished)
	{
		return;
	}
	
	bIsMiniGameFinished = true;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MiniGameTimerHandle);
	}
	
	if (ADDMiniGameStateBase* MiniGameState = GetMiniGameState())
	{
		// 게임이 종료된 상태로 갱신
		MiniGameState->SetMiniGameState(DDMiniGameplayTags::State_Finishing);
		
		// RuleSet이 있다면 RuleSet의 규칙에 따라 랭킹 정리
		if (RuleSet != nullptr)
		{
			RuleSet->ResolveRanking(MiniGameState);
		}
		
		MiniGameState->SetRemainingTimeSeconds(0.f);
		MiniGameState->SetMiniGameState(DDMiniGameplayTags::State_Completed);
	}
	
	// 게임 결과를 생성
	FMiniGameResult Result = BuildMiniGameResult();
	Result.FinishReason = Reason;
	
	if (GetGameInstance() != nullptr)
	{
		if (UDDMiniGameManager* MiniGameManager = GetGameInstance()->GetSubsystem<UDDMiniGameManager>())
		{
			// 게임 결과 저장과 보드 맵 복귀는 Manager에서 일괄적으로 관리하도록 함
			MiniGameManager->CommitMiniGameResult(Result);
		}
	}
}

void ADDMiniGameModeBase::InitializeRuleSet()
{
	// 일단 RuleSet을 한 번 nullptr로 초기화
	RuleSet = nullptr;
	
	if (GetGameInstance() == nullptr)
	{
		return;
	}

	
	UDDMiniGameManager* MiniGameManager = GetGameInstance()->GetSubsystem<UDDMiniGameManager>();
	UDDMiniGameDefinition* Definition = MiniGameManager != nullptr ? MiniGameManager->GetActiveDefinition() : nullptr;
	if (Definition == nullptr || Definition->RuleSetClass == nullptr)
	{
		return;
	}

	// 미니게임 데이터에 RuleSet이 있는 경우 Initialize
	RuleSet = NewObject<UDDMiniGameRuleSet>(this, Definition->RuleSetClass);
	if (RuleSet != nullptr)
	{
		RuleSet->InitializeRules(ActiveSetup);
	}
}
