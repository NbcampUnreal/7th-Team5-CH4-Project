#include "Base/MiniGame/DDMiniGameModeBase.h"

#include "Base/MiniGame/DDMiniGameRuleSet.h"
#include "Base/MiniGame/DDMiniGameStateBase.h"
#include "Base/Player/DDBasePlayerController.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "System/MiniGame/DDMiniGameDefinition.h"
#include "System/MiniGame/DDMiniGameManager.h"
#include "TimerManager.h"
#include "Common/DDLogManager.h"
#include "GameFramework/PlayerState.h"

static const TArray<FName> MiniGameSpawnTags =
{
	TEXT("SpawnSlot_MiniGame_Slot0"),
	TEXT("SpawnSlot_MiniGame_Slot1"),
	TEXT("SpawnSlot_MiniGame_Slot2"),
	TEXT("SpawnSlot_MiniGame_Slot3"),
};

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
	if (MiniGameManager == nullptr || !MiniGameManager->HasActiveMiniGame() || !MiniGameManager->
		IsActiveMiniGameWorld(GetWorld()))
	{
		return;
	}

	// Seamless Travel 직후 플레이어 배치 및 BeginPlay 초기화가 같은 데이터를 보도록 동기화
	LoadActiveMiniGameData();

	// 미니게임 세팅 Initialize
	InitializeMiniGame(MiniGameManager->GetActiveSetup(), MiniGameManager->GetActiveParticipants());
	// RuleSet이 있다면 Initialize
	InitializeRuleSet();
	// 모든 미니게임은 준비 완료 이후에만 시작되도록 공통 ready 상태를 초기화
	InitializeReadyStates();

	MiniGameManager->NotifyMiniGameStarted();
}

void ADDMiniGameModeBase::HandleSeamlessTravelPlayer(AController*& C)
{
	AGameModeBase::HandleSeamlessTravelPlayer(C);

	// Seamless Travel 시점에는 BeginPlay보다 먼저 호출될 수 있다고 함 
	// 따라서 런타임 데이터를 한 번 동기화 시켜 줌
	LoadActiveMiniGameData();

	// 넘어온 Controller가 실제 플레이어 컨트롤러인지 확인
	APlayerController* PlayerController = Cast<APlayerController>(C);
	if (PlayerController == nullptr)
	{
		return;
	}

	// 이전 맵 Pawn은 미니게임 전용 스폰 정책과 충돌할 수 있어 제거
	if (APawn* OldPawn = PlayerController->GetPawn())
	{
		OldPawn->Destroy();
	}

	// 현재 플레이어가 미니게임 참가자인지 참가자 목록에서 찾음
	const FMiniGameParticipantInfo* Participant = FindParticipantInfo(PlayerController->PlayerState);
	if (Participant == nullptr || ShouldSpawnAsSpectator(*Participant))
	{
		// 참가자가 아니거나 관전자 처리 대상이면 관전자(SpectatorPawn)가 됨
		SpawnSpectatorPawn(PlayerController);
		return;
	}

	if (AActor* StartSpot = ChoosePlayerStart(PlayerController))
	{
		RestartPlayerAtPlayerStart(PlayerController, StartSpot);
	}
	else
	{
		RestartPlayer(PlayerController);
	}

	if (ADDBasePlayerController* BasePlayerController = Cast<ADDBasePlayerController>(PlayerController))
	{
		ApplyMiniGameInput(BasePlayerController);
	}
	
	// 준비 위젯 띄우기 
	BroadcastOpenPopUp(DDGameplayTags::MiniGame_UI_ReadyPopUp);
}

AActor* ADDMiniGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	const APlayerState* PlayerState = Player != nullptr ? Player->PlayerState : nullptr;
	const FMiniGameParticipantInfo* Participant = FindParticipantInfo(PlayerState);
	if (Participant == nullptr)
	{
		return Super::ChoosePlayerStart_Implementation(Player);
	}

	if (!MiniGameSpawnTags.IsValidIndex(Participant->SlotIndex))
	{
		return Super::ChoosePlayerStart_Implementation(Player);
	}

	// 만들어둔 태그배열을 인덱스로 찾아서 해당하는 태그를 찾음
	const FName& SlotTag = MiniGameSpawnTags[Participant->SlotIndex];
	
	// 월드에 배치된 모든 PlayerStart 액터를 순회해서 해당하는 SlotTag를 가진 PlayerStart를 찾아서 반환
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(SlotTag))
		{
			return *It;
		}
	}

	return Super::ChoosePlayerStart_Implementation(Player);
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
		const float RemainingTimeSeconds = ActiveSetup.TimeLimitSeconds > 0.f ?
			FMath::Max(0.f, ActiveSetup.TimeLimitSeconds - ElapsedTimeSeconds) : 0.f;

		MiniGameState->SetRemainingTimeSeconds(RemainingTimeSeconds);
	}

	if (RuleSet != nullptr && RuleSet->ShouldFinishGame(GetMiniGameState()))
	{
		// 개별 룰셋이 종료 조건을 만족했다고 판단하면 남은 시간과 관계없이 즉시 종료
		FinishGame(DDGameplayTags::FinishReason_MiniGame_Completed);
		return;
	}

	// 제한 시간이 끝나면 게임을 TimeOver 태그에 의해 종료
	if (ActiveSetup.TimeLimitSeconds > 0.f && ElapsedTimeSeconds >= ActiveSetup.TimeLimitSeconds)
	{
		FinishGame(DDGameplayTags::FinishReason_MiniGame_TimeOver);
	}
}

void ADDMiniGameModeBase::InitializeMiniGame(
	const FMiniGameSetup& InSetup,
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
		MiniGameState->SetMiniGameSetup(ActiveSetup);
		MiniGameState->SetParticipants(ActiveParticipants);
		MiniGameState->SetRemainingTimeSeconds(ActiveSetup.TimeLimitSeconds);
		MiniGameState->SetReadyPlayerCount(0);
		MiniGameState->SetTotalParticipantCount(ActiveParticipants.Num());
		MiniGameState->SetReadyEntries(TArray<FMiniGameReadyEntry>());
		MiniGameState->SetMiniGameState(DDGameplayTags::State_MiniGame_Preparing);
		MiniGameState->SetScoreBoard(TArray<FMiniGameScoreEntry>());
		MiniGameState->NotifyMiniGameSetupReady();
	}
}

void ADDMiniGameModeBase::LoadActiveMiniGameData()
{
	if (GetGameInstance() == nullptr)
	{
		return;
	}
	
	// 미니게임 매니저로부터 활성화된 미니게임의 정보를 받아오는 작업
	if (UDDMiniGameManager* MiniGameManager = GetGameInstance()->GetSubsystem<UDDMiniGameManager>())
	{
		if (MiniGameManager->HasActiveMiniGame())
		{
			ActiveSetup = MiniGameManager->GetActiveSetup();
			ActiveParticipants = MiniGameManager->GetActiveParticipants();
		}
	}
}

const FMiniGameParticipantInfo* ADDMiniGameModeBase::FindParticipantInfo(const APlayerState* PlayerState) const
{
	if (PlayerState == nullptr)
	{
		return nullptr;
	}

	// 참가자 정보 검색
	for (const FMiniGameParticipantInfo& Participant : ActiveParticipants)
	{
		if (Participant.PlayerId == PlayerState->GetPlayerId())
		{
			return &Participant;
		}
	}

	return nullptr;
}

bool ADDMiniGameModeBase::ShouldSpawnAsSpectator(const FMiniGameParticipantInfo& Participant) const
{
	// 기본적오르논 준비되지 않았거나 연결이 끊긴 참가자를 관전자로 처리
	return !Participant.bReady || !Participant.bConnected;
}

void ADDMiniGameModeBase::StartMiniGame()
{
	if (bIsMiniGameStarted || bIsMiniGameFinished)
	{
		return;
	}
	
	// 팝업 제거 
	BroadcastClosePopUp(DDGameplayTags::MiniGame_UI_ReadyPopUp); 
	
	// UI 세팅 
	BroadcastGameLayer(DDGameplayTags::MiniGame_UI_Main);

	// 플레이 시작 및 시작 시점을 기록
	bIsMiniGameStarted = true;
	ElapsedTimeSeconds = 0.0f;

	// GameState에서 시작 상태 태그를 갱신 & 게임의 남은 시간을 게임 데이터의 제한시간으로 갱신
	if (ADDMiniGameStateBase* MiniGameState = GetMiniGameState())
	{
		MiniGameState->SetMiniGameState(DDGameplayTags::State_MiniGame_Playing);
		MiniGameState->SetRemainingTimeSeconds(ActiveSetup.TimeLimitSeconds);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			MiniGameTimerHandle, 
			this, 
			&ADDMiniGameModeBase::UpdateMiniGameTime,
			TimeUpdateIntervalSeconds, 
			true
		);
	}
	
	
}

void ADDMiniGameModeBase::FinishMiniGame()
{
	// 기본적으로 사용하는 종료 요청
	FinishGame(DDGameplayTags::FinishReason_MiniGame_Completed);
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

void ADDMiniGameModeBase::RefreshRanking(ADDMiniGameStateBase* MiniGameState) const
{
	if (MiniGameState == nullptr)
	{
		return;
	}

	if (RuleSet != nullptr)
	{
		RuleSet->ResolveRanking(MiniGameState);
		return;
	}

	// 내림차순 정렬
	TArray<FMiniGameScoreEntry> SortedScoreBoard = MiniGameState->GetScoreBoard();
	SortedScoreBoard.Sort([](const FMiniGameScoreEntry& Left, const FMiniGameScoreEntry& Right)
	{
		return Left.Score > Right.Score;
	});

	for (int32 Index = 0; Index < SortedScoreBoard.Num(); ++Index)
	{
		SortedScoreBoard[Index].Rank = Index + 1;
	}

	MiniGameState->SetScoreBoard(SortedScoreBoard);
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

	RefreshRanking(MiniGameState);
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
		MiniGameState->SetMiniGameState(DDGameplayTags::State_MiniGame_Finishing);

		// 종료 직전에 현재 점수판 기준으로 최종 랭킹을 확정
		RefreshRanking(MiniGameState);

		MiniGameState->SetRemainingTimeSeconds(0.f);
		MiniGameState->SetMiniGameState(DDGameplayTags::State_MiniGame_Completed);
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

void ADDMiniGameModeBase::ApplyMiniGameInput(ADDBasePlayerController* PlayerController)
{
	if (PlayerController == nullptr || GetGameInstance() == nullptr)
	{
		return;
	}
	
	const UDDMiniGameManager* MiniGameManager = GetGameInstance()->GetSubsystem<UDDMiniGameManager>();
	const UDDMiniGameDefinition* Definition = MiniGameManager != nullptr ? MiniGameManager->GetActiveDefinition() : nullptr;
	
	if (Definition == nullptr)
	{
		return;
	}
	
	// Client RPC를 호출해서 Client에 적용
	PlayerController->Client_ApplyInput(Definition->MappingContextClass);
}

void ADDMiniGameModeBase::InitializeReadyStates()
{
	ReadyStates.Reset();

	for (const FMiniGameParticipantInfo& Participant : ActiveParticipants)
	{
		ReadyStates.Add(Participant.PlayerId, false);
	}

	UpdateReadyState();
}

void ADDMiniGameModeBase::UpdateReadyState()
{
	ADDMiniGameStateBase* MiniGameState = GetMiniGameState();
	if (MiniGameState == nullptr)
	{
		return;
	}

	int32 ReadyPlayerCount = 0;
	TArray<FMiniGameReadyEntry> ReadyEntries;
	ReadyEntries.Reserve(ActiveParticipants.Num());

	for (const FMiniGameParticipantInfo& Participant : ActiveParticipants)
	{
		const bool* bReady = ReadyStates.Find(Participant.PlayerId);
		const bool bIsReady = bReady != nullptr && *bReady;
		if (bIsReady)
		{
			++ReadyPlayerCount;
		}

		FMiniGameReadyEntry ReadyEntry;
		ReadyEntry.PlayerId = Participant.PlayerId;
		ReadyEntry.DisplayName = Participant.DisplayName;
		ReadyEntry.bReady = bIsReady;
		ReadyEntries.Add(ReadyEntry);
	}

	MiniGameState->SetReadyPlayerCount(ReadyPlayerCount);
	MiniGameState->SetTotalParticipantCount(ActiveParticipants.Num());
	MiniGameState->SetReadyEntries(ReadyEntries);
}

bool ADDMiniGameModeBase::AreAllParticipantsReady() const
{
	if (ActiveParticipants.Num() == 0)
	{
		return false;
	}

	for (const FMiniGameParticipantInfo& Participant : ActiveParticipants)
	{
		const bool* bReady = ReadyStates.Find(Participant.PlayerId);
		if (bReady == nullptr || !*bReady)
		{
			return false;
		}
	}

	return true;
}

void ADDMiniGameModeBase::TryStartMiniGame()
{
	LOG_KMS(Warning, TEXT("Try Start MiniGame"));
	if (bIsMiniGameStarted || bIsMiniGameFinished)
	{
		return;
	}

	if (AreAllParticipantsReady())
	{
		StartMiniGame();
		 
	}
	else
	{
		LOG_KMS(Error, TEXT("Failed to start mini-game mode"));
	}
}

void ADDMiniGameModeBase::SetPlayerReady(APlayerState* PlayerState, bool bReady)
{
	LOG_KMS(Warning, TEXT("SetPlayerReady"));
	if (!HasAuthority() || bIsMiniGameStarted || bIsMiniGameFinished || PlayerState == nullptr)
	{
		return;
	}

	if (FindParticipantInfo(PlayerState) == nullptr)
	{
		return;
	}

	ReadyStates.FindOrAdd(PlayerState->GetPlayerId()) = bReady;
	UpdateReadyState();
	TryStartMiniGame();
}

void ADDMiniGameModeBase::HandlePlayerReady(APlayerController* PlayerController, bool bReady)
{
	if (!HasAuthority() || bIsMiniGameStarted || bIsMiniGameFinished || PlayerController == nullptr)
	{
		return;
	}
	
	APlayerState* PS = PlayerController->GetPlayerState<APlayerState>(); 
	
	SetPlayerReady(PS, bReady);
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
