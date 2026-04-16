#include "BoardGame/Game/DDBoardGameMode.h"
#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "Base/Game/DDGameStateBase.h"
#include "System/DDGameInstance.h"
#include "TimerManager.h"
#include "AbilitySystem/Attributes/DDPointSet.h"
#include "Base/Player/DDBasePlayerController.h"
#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/DDTileManager.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "Common/DDLogManager.h"
#include "System/DDGameplayTags.h"
#include "System/MiniGame/DDMiniGameManager.h"
#include "GameplayEffect.h"
#include "BoardGame/DDTile.h"
#include "BoardGame/Game/DDBoardGameState.h"

ADDBoardGameMode::ADDBoardGameMode()
{
}

void ADDBoardGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// 시작할 때 미리 캐싱
    CachedBoardGameState = GetGameState<ADDBoardGameState>();

    if (CachedBoardGameState)
    {
        // [Bridge 패턴] GameInstance에서 GameState로 데이터 복사
        if (UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance()))
        {
            CachedBoardGameState->CurrentRound = GameInstance->CurrentRound;
            CachedBoardGameState->MaxTrophy = GameInstance->MaxTrophy;
            CachedBoardGameState->MaxRound = GameInstance->MaxRound;
        }
    }
    else
    {
        LOG_CJH(Log, TEXT("DDBoardGameState 캐싱 실패!"));
    }
	
	LOG_CJH(Log, TEXT("[BeginPlay] 보드게임 맵 진입 완료. 메인 타이머 시작."));
	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.f, true);
}

void ADDBoardGameMode::OnMainTimerElapsed()
{
	ADDGameStateBase* GameStateBase = GetGameState<ADDGameStateBase>();
	if (!IsValid(GameStateBase)) return;

	if (!GameStateBase->MatchStateTag.IsValid())
	{
		if (AlivePlayerControllers.Num() >= GameStateBase->MinPlayerCount)
		{
			bool bAllPlayerStatesReady = true;
			for (APlayerController* PC : AlivePlayerControllers)
			{
				if (!IsValid(PC->PlayerState))
				{
					bAllPlayerStatesReady = false;
					break;
				}
			}

			if (bAllPlayerStatesReady)
			{
				LOG_CJH(Log, TEXT("[GameLoop] %d명 접속 완료! 보드게임 Init 상태로 진입합니다."), GameStateBase->MinPlayerCount);
				SetMatchState(DDGameplayTags::State_BoardGame_Init);
			}
		}
		return;
	}

	if (GameStateBase->MatchStateTag == DDGameplayTags::State_BoardGame_PlayerTurn)
	{
		if (CachedBoardGameState->StateTimer > 0)
		{
			CachedBoardGameState->StateTimer--;
			LOG_CJH(Log, TEXT("현재 턴 남은 시간: %d"), CachedBoardGameState->StateTimer);

			if (GetNetMode() != NM_DedicatedServer)
			{
				CachedBoardGameState->OnStateTimerChanged.Broadcast(CachedBoardGameState->StateTimer);
			}
			
			if (CachedBoardGameState->StateTimer == 0)
			{
				LOG_CJH(Log, TEXT("[TimeOut] 턴 제한 시간 초과! 다음 플레이어로 턴을 강제 전환합니다."));
				CurrentTurnPlayerIndex++;
				SetMatchState(DDGameplayTags::State_BoardGame_PlayerTurn);
			}
		}
	}
	else if (GameStateBase->MatchStateTag == DDGameplayTags::State_BoardGame_RoundEnd)
	{
		if (CachedBoardGameState->StateTimer > 0)
		{
			CachedBoardGameState->StateTimer--;
			if (CachedBoardGameState->StateTimer == 0)
			{
				LOG_CJH(Log, TEXT("[Travel] 대기 시간 종료. 미니게임을 시작합니다."));

				TArray<APlayerState*> MiniGamePlayers;
				for (APlayerController* PlayerController : AlivePlayerControllers)
				{
					if (IsValid(PlayerController) && PlayerController->PlayerState)
					{
						MiniGamePlayers.Add(PlayerController->PlayerState);
					}
				}
				UDDMiniGameManager* MiniGameManager = GetGameInstance()->GetSubsystem<UDDMiniGameManager>();
				if (IsValid(MiniGameManager))
				{
					LOG_CJH(Log, TEXT("[Travel] 미니게임에 진입합니다."));
					MiniGameManager->RequestStartRandomMiniGame(MiniGamePlayers);
				}
			}
		}
	}
}

void ADDBoardGameMode::SetMatchState(FGameplayTag NewStateTag)
{
	ADDGameStateBase* GameStateBase = GetGameState<ADDGameStateBase>();
	if (!IsValid(GameStateBase)) return;

	LOG_CJH(Log, TEXT("================================================="));
	LOG_CJH(Log, TEXT("[상태 전환] 새로운 상태: %s"), *NewStateTag.ToString());
	LOG_CJH(Log, TEXT("================================================="));

	GameStateBase->MatchStateTag = NewStateTag;

	for (APlayerController* PlayerController : AlivePlayerControllers)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromPlayer(PlayerController))
		{
			ASC->RemoveActiveEffectsWithGrantedTags(CurrentAppliedTags);

			if (TSubclassOf<UGameplayEffect>* EffectClassPtr = MatchStateEffectClasses.Find(NewStateTag))
			{
				if (*EffectClassPtr)
				{
					FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
					Context.AddSourceObject(this);
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(*EffectClassPtr, 1.0f, Context);
					if (SpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
		}
	}

	CurrentAppliedTags = FGameplayTagContainer(NewStateTag);

	if (NewStateTag == DDGameplayTags::State_BoardGame_Init)
	{
		SortPlayersByTurnOrder();
		LOG_CYS(Warning, TEXT("[GM] 보드게임 전체 초기화 시작"));

		ADDTileManager* TM = nullptr;
		for (TActorIterator<ADDTileManager> It(GetWorld()); It; ++It)
		{
			TM = *It;
			break;
		}

		if (TM) TM->InitializeTiles();

		for (APlayerController* PC : AlivePlayerControllers)
		{
			if (auto PS = PC->GetPlayerState<ADDBasePlayerState>()) PS->InitTile();
		}

		for (APlayerController* PC : AlivePlayerControllers)
		{
			if (!PC->GetPawn()) RestartPlayer(PC);
			if (auto Char = Cast<ADDBoardGameCharacter>(PC->GetPawn())) Char->InitLocation();
			if (auto DDPC = Cast<ADDBasePlayerController>(PC)) DDPC->Client_ApplyState(NewStateTag);
		}

		LOG_CYS(Warning, TEXT("[GM] 보드게임 전체 초기화 완료"));

		UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance());

		if (IsValid(GameInstance) && GameInstance->CurrentRound == 1)
		{
			//1라운드 시퀀서 재생 지시
			LOG_CYS(Warning, TEXT("1라운드 인트로 시퀀서 재생해라"));
			ADDBoardGameState* GS = GetGameState<ADDBoardGameState>();
			if (GS)
			{
				GS->Multicast_PlaySequence();
				// 시퀀서 끝나면 GS에서 턴 지시함.
			}
		}
		else
		{
			CheckWinCondition();
		}
	}
	else if (NewStateTag == DDGameplayTags::State_BoardGame_PlayerTurn)
	{
		StartNextPlayerTurn();
	}
	else if (NewStateTag == DDGameplayTags::State_BoardGame_Ending)
	{
		CalculateFinalWinner();
	}
	else if (NewStateTag == DDGameplayTags::State_BoardGame_RoundEnd)
	{
		CachedBoardGameState->StateTimer = 3;
		LOG_CJH(Log, TEXT("[라운드 종료] 3초 뒤 미니게임으로 이동합니다."));
		// 현재 타일 위치 시작 위치로 초기화
		for (APlayerController* PC : AlivePlayerControllers)
		{
			if (auto PS = PC->GetPlayerState<ADDBasePlayerState>())
			{
				if (IsValid(PS->CurrentTile))
				{
					PS->StartTileName = PS->CurrentTile->TileRowName;

					LOG_CYS(Log, TEXT("[RoundEnd] Save Tile: %s"),
					        *PS->StartTileName.ToString());
				}
				else
				{
					LOG_CYS(Error, TEXT("[RoundEnd] CurrentTile INVALID"));
				}
			}
		}
	}
}

void ADDBoardGameMode::Logout(AController* Exiting)
{
	APlayerController* ExitingPC = Cast<APlayerController>(Exiting);
    if (IsValid(ExitingPC))
    {
        // 1. 현재 턴인 사람이 나가는 것인지 확인
        bool bIsCurrentTurnPlayer = false;
        if (AlivePlayerControllers.IsValidIndex(CurrentTurnPlayerIndex) && 
            AlivePlayerControllers[CurrentTurnPlayerIndex] == ExitingPC)
        {
            bIsCurrentTurnPlayer = true;
        }

        // 2. 명단에서 제거
        AlivePlayerControllers.Remove(ExitingPC);

        // 3. 비상 탈출 로직: 나간 사람이 현재 턴의 주인이었다면?
        if (bIsCurrentTurnPlayer)
        {
            LOG_CJH(Warning, TEXT("현재 턴인 플레이어가 이탈했습니다! 강제로 다음 턴으로 넘깁니다."));
            
            // 기존 타이머들이 돌고 있다면 모두 취소
            GetWorldTimerManager().ClearTimer(TurnTransitionTimerHandle);
            
            // 배열에서 제거되었으므로, 같은 인덱스를 유지하면 자연스레 다음 사람을 가리킴
            // (마지막 사람이었다면 RoundEnd로 넘어가도록 조치 필요)
            if (CurrentTurnPlayerIndex >= AlivePlayerControllers.Num())
            {
                CurrentTurnPlayerIndex = 0;
                SetMatchState(DDGameplayTags::State_BoardGame_RoundEnd);
            }
            else
            {
                SetMatchState(DDGameplayTags::State_BoardGame_PlayerTurn);
            }
        }
    }

    Super::Logout(Exiting);
	Super::Logout(Exiting);
}

void ADDBoardGameMode::CheckWinCondition()
{
	LOG_CJH(Log, TEXT("[CheckWinCondition] 승리 조건 및 최대 라운드 도달 여부를 검사합니다."));
	ADDGameStateBase* GameStateBase = GetGameState<ADDGameStateBase>();
	UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance());

	if (!IsValid(GameStateBase) || !IsValid(GameInstance)) return;

	bool bHasTrophyWinner = false;

	for (APlayerState* PlayerState : GameStateBase->PlayerArray)
	{
		if (ADDBasePlayerState* BasePlayerState = Cast<ADDBasePlayerState>(PlayerState))
		{
			if (BasePlayerState->GetPointSet() && BasePlayerState->GetPointSet()->GetTrophy() >= CachedBoardGameState->MaxTrophy)
			{
				LOG_CJH(Log, TEXT("목표 트로피 도달자 발생! (닉네임: %s)"),
				        *BasePlayerState->PlayerGameData.PlayerDisplayName.ToString());
				bHasTrophyWinner = true;
				break;
			}
		}
	}

	if (bHasTrophyWinner || GameInstance->CurrentRound > CachedBoardGameState->MaxRound)
	{
		LOG_CJH(Log, TEXT("[CheckWinCondition] 게임 종료 조건 충족. Ending 페이즈로 전환합니다."));
		SetMatchState(DDGameplayTags::State_BoardGame_Ending);
	}
	else
	{
		LOG_CJH(Log, TEXT("[CheckWinCondition] 게임을 계속 진행합니다. (현재 라운드: %d)"), GameInstance->CurrentRound);
		CurrentTurnPlayerIndex = 0;
		SetMatchState(DDGameplayTags::State_BoardGame_PlayerTurn);
	}
}

void ADDBoardGameMode::StartNextPlayerTurn()
{
	if (CurrentTurnPlayerIndex >= AlivePlayerControllers.Num())
	{
		CurrentTurnPlayerIndex = 0;
		UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance());
		if (IsValid(GameInstance))
		{
			GameInstance->CurrentRound++;
			if (CachedBoardGameState)
			{
				CachedBoardGameState->CurrentRound = GameInstance->CurrentRound;
			}
			LOG_CJH(Log, TEXT("모든 플레이어의 턴 종료. 라운드를 넘깁니다."));
		}

		SetMatchState(DDGameplayTags::State_BoardGame_RoundEnd);
		return;
	}

	APawn* ActivePawn = nullptr;
	if (AlivePlayerControllers.IsValidIndex(CurrentTurnPlayerIndex))
	{
		ActivePawn = AlivePlayerControllers[CurrentTurnPlayerIndex]->GetPawn();
	}

	for (int32 i = 0; i < AlivePlayerControllers.Num(); ++i)
	{
		APlayerController* PlayerController = AlivePlayerControllers[i];
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromPlayer(PlayerController);
		if (!ASC) continue;

		FGameplayTagContainer TurnTagsToRemove;
		TurnTagsToRemove.AddTag(DDGameplayTags::State_BoardGame_TurnActive);
		TurnTagsToRemove.AddTag(DDGameplayTags::State_BoardGame_TurnWaiting);
		TurnTagsToRemove.AddTag(DDGameplayTags::State_BoardGame_HasUsedItem);
		ASC->RemoveActiveEffectsWithGrantedTags(TurnTagsToRemove);

		TSubclassOf<UGameplayEffect> EffectToApply = (i == CurrentTurnPlayerIndex)
			                                             ? TurnActiveEffectClass
			                                             : TurnWaitingEffectClass;

		if (EffectToApply)
		{
			FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
			Context.AddSourceObject(this);
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectToApply, 1.0f, Context);
			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
		

		ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(PlayerController);
		if (IsValid(DDPC))
		{
			if (i == CurrentTurnPlayerIndex)
			{
				DDPC->Client_SetMouseCursorVisible(true);
				CachedBoardGameState->StateTimer = CachedBoardGameState->MaxStateTimer;
				LOG_CJH(Log, TEXT("▶ [%d]번 플레이어 턴 시작! (제한시간 %d초)"), i, CachedBoardGameState->MaxStateTimer);
				SetTurnPhase(DDGameplayTags::State_TurnPhase_BeforeDice);
			}
			else
			{
				DDPC->Client_SetMouseCursorVisible(false);
			}
		}

	}

	Super::FocusAllCamerasOnTarget(ActivePawn);
}

void ADDBoardGameMode::NotifyDiceRolled()
{
	LOG_CJH(Log, TEXT("[Notify] 주사위 굴림. BeforeDice를 제거하고 Moving 페이즈로 전환."));
	SetTurnPhase(DDGameplayTags::State_TurnPhase_Moving);
	CachedBoardGameState->StateTimer = -1;
}

void ADDBoardGameMode::NotifyMovementFinished()
{
	LOG_CJH(Log, TEXT("[Notify] 이동 완료. Moving 태그를 제거하고 %.0f초 뒤 턴을 넘깁니다."), TurnTransitionTimer);
	SetTurnPhase(FGameplayTag::EmptyTag);
	GetWorldTimerManager().SetTimer(TurnTransitionTimerHandle, this, &ThisClass::ExecuteNextTurnTransition,
	                                TurnTransitionTimer, false);
}

void ADDBoardGameMode::HandleRespawn(AController* TargetController)
{
	Super::HandleRespawn(TargetController);
	// State의 currentTile을 Tile01로
	if (!TargetController) return;

	ADDBasePlayerState* PS = TargetController->GetPlayerState<ADDBasePlayerState>();
	if (!PS) return;
	PS->StartTileName = FName("Tile01");
	PS->InitTile();
}

void ADDBoardGameMode::ExecuteNextTurnTransition()
{
	LOG_CJH(Log, TEXT("[Timer] %.0f초 대기 완료. 다음 플레이어로 턴을 전환합니다."), TurnTransitionTimer);
	CurrentTurnPlayerIndex++;
	SetMatchState(DDGameplayTags::State_BoardGame_PlayerTurn);
}

void ADDBoardGameMode::SetTurnPhase(FGameplayTag NewPhaseTag)
{
	if (!AlivePlayerControllers.IsValidIndex(CurrentTurnPlayerIndex)) return;

	APlayerController* PlayerController = AlivePlayerControllers[CurrentTurnPlayerIndex];
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromPlayer(PlayerController);
	if (!ASC) return;

	if (CurrentTurnPhaseTag.IsValid())
	{
		FGameplayTagContainer PhaseTagToRemove(CurrentTurnPhaseTag);
		ASC->RemoveActiveEffectsWithGrantedTags(PhaseTagToRemove);
	}

	CurrentTurnPhaseTag = NewPhaseTag;

	if (CurrentTurnPhaseTag.IsValid())
	{
		if (TSubclassOf<UGameplayEffect>* EffectClassPtr = TurnPhaseEffectClasses.Find(NewPhaseTag))
		{
			if (*EffectClassPtr)
			{
				FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
				Context.AddSourceObject(this);
				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(*EffectClassPtr, 1.0f, Context);
				if (SpecHandle.IsValid())
				{
					ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		}
		LOG_CJH(Log, TEXT("   └ [TurnPhase] 페이즈 갱신: %s"), *NewPhaseTag.ToString());
	}
}

void ADDBoardGameMode::CalculateFinalWinner()
{
	if (!IsValid(CachedBoardGameState)) return;

    // 1. AlivePlayerControllers에서 참여 중인 PlayerState 수집
    TArray<ADDBasePlayerState*> RankCandidates;
    for (APlayerController* PC : AlivePlayerControllers)
    {
        if (PC && PC->PlayerState)
        {
            if (ADDBasePlayerState* PS = Cast<ADDBasePlayerState>(PC->PlayerState))
            {
                // PointSet이 유효한지 확인 후 추가
                if (PS->GetPointSet())
                {
	                RankCandidates.Add(PS);
                }
            }
        }
    }

    // 2. 정렬 (람다식: 트로피 우선, 코인 차선)
    RankCandidates.Sort([](const ADDBasePlayerState& A, const ADDBasePlayerState& B)
    {
        float TrophyA = A.GetPointSet()->GetTrophy();
        float TrophyB = B.GetPointSet()->GetTrophy();
        
        if (TrophyA != TrophyB) return TrophyA > TrophyB; 
        
        return A.GetPointSet()->GetCoin() > B.GetPointSet()->GetCoin();
    });

    // 3. UI용 구조체 배열로 변환
    TArray<FFinalRankData> FinalResults;
    for (int32 i = 0; i < RankCandidates.Num(); ++i)
    {
        FFinalRankData Data;
        Data.Rank = i + 1;
        Data.PlayerName = RankCandidates[i]->PlayerGameData.PlayerDisplayName;
        Data.Trophy = (int32)RankCandidates[i]->GetPointSet()->GetTrophy();
        Data.Coin = (int32)RankCandidates[i]->GetPointSet()->GetCoin();
        
        FinalResults.Add(Data);
    	
    	LOG_CJH(Log, TEXT("%d등 플레이어: %s | 트로피 개수: %d, 코인 개수: %d"), i + 1, *Data.PlayerName.ToString(), Data.Trophy, Data.Coin);
    }

    // 4. GameState에 저장
    CachedBoardGameState->FinalRankings = FinalResults;

    // 5. 결과 상태로 전환
    SetMatchState(DDGameplayTags::State_BoardGame_End);
}

void ADDBoardGameMode::SortPlayersByTurnOrder()
{
	for (int32 i = 0; i < AlivePlayerControllers.Num(); ++i)
	{
		if (ADDBasePlayerState* BasePlayerState = AlivePlayerControllers[i]->GetPlayerState<ADDBasePlayerState>())
		{
			if (BasePlayerState->PlayerGameData.TurnOrder < 0)
			{
				BasePlayerState->PlayerGameData.TurnOrder = i + 100;
			}
		}
	}

	AlivePlayerControllers.Sort([](const TObjectPtr<APlayerController>& A, const TObjectPtr<APlayerController>& B)
	{
		ADDBasePlayerState* PlayerStateA = A ? A->GetPlayerState<ADDBasePlayerState>() : nullptr;
		ADDBasePlayerState* PlayerStateB = B ? B->GetPlayerState<ADDBasePlayerState>() : nullptr;

		if (PlayerStateA && PlayerStateB)
		{
			return PlayerStateA->PlayerGameData.TurnOrder < PlayerStateB->PlayerGameData.TurnOrder;
		}
		return false;
	});

	LOG_CJH(Log, TEXT("====================================="));
	LOG_CJH(Log, TEXT("이번 라운드의 턴 순서가 결정되었습니다!"));
	for (int32 i = 0; i < AlivePlayerControllers.Num(); ++i)
	{
		if (ADDBasePlayerState* BasePlayerState = AlivePlayerControllers[i]->GetPlayerState<ADDBasePlayerState>())
		{
			LOG_CJH(Log, TEXT(" [%d] %s (TurnOrder: %d)"), i + 1,
			        *BasePlayerState->PlayerGameData.PlayerDisplayName.ToString(),
			        BasePlayerState->PlayerGameData.TurnOrder);
		}
	}
	LOG_CJH(Log, TEXT("====================================="));
}
