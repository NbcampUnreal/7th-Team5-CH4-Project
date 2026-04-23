#include "BoardGame/Game/DDBoardGameMode.h"
#include "AbilitySystemComponent.h"
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

ADDBoardGameMode::ADDBoardGameMode() {}

void ADDBoardGameMode::BeginPlay()
{
	Super::BeginPlay();
	
    CachedBoardGameState = GetGameState<ADDBoardGameState>();
    if (CachedBoardGameState)
    {
    	UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance());
        if (GameInstance)
        {
            CachedBoardGameState->SetCurrentRound(GameInstance->CurrentRound);
        }
    }
    else
    {
        LOG_CJH(Log, TEXT("DDBoardGameState 캐싱 실패!"));
    }
	
	LOG_CJH(Log, TEXT("[BeginPlay] 보드게임 맵 진입 완료. 메인 타이머 시작."));
	
	GetWorld()->GetTimerManager().SetTimer(
		MainTimerHandle, 
		this,
		&ThisClass::OnMainTimerElapsed, 
		1.f, 
		true
	);
}

void ADDBoardGameMode::OnMainTimerElapsed()
{
	if (!IsValid(CachedBoardGameState)) return;
	
	FGameplayTag CurrentMatchState = CachedBoardGameState->GetBoardMatchState();

	if (!CurrentMatchState.IsValid())
	{
		TickState_WaitingForPlayers();
	}
	else if (CurrentMatchState == DDGameplayTags::State_BoardGame_PlayerTurn)
	{
		TickState_PlayerTurn();
	}
	else if (CurrentMatchState == DDGameplayTags::State_BoardGame_RoundEnd)
	{
		TickState_RoundEnd();
	}
}

void ADDBoardGameMode::TickState_WaitingForPlayers()
{
	UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance());
	if (!GameInstance) return;

	// 1. 로비에서 넘어오기로 약속된 '총 예상 인원'
	int32 ExpectedPlayers = GameInstance->ExpectedPlayerCount;

	// 혹시 값이 세팅 안 되어있을 경우를 대비한 방어 코드
	if (ExpectedPlayers <= 0) ExpectedPlayers = CachedBoardGameState->GetMinPlayerCount();

	// 2. 아직 약속된 인원수만큼 서버에 접속조차 못 했다면 무조건 대기
	if (AlivePlayerControllers.Num() < ExpectedPlayers) 
	{
		return; 
	}

	// 3. 접속한 인원 중 로딩이 완벽하게 끝난 인원수 카운트
	int32 FullyLoadedPlayerCount = 0;

	for (APlayerController* PC : AlivePlayerControllers)
	{
		if (ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(PC))
		{
			ADDBasePlayerState* PS = DDPC->GetCachedPlayerState();
			
			// PlayerState 유효 + 클라이언트 로딩 완료 통보 + 캐릭터 빙의 완료 확인
			if (PS && PS->bHasClientLoaded && IsValid(DDPC->GetPawn()))
			{
				FullyLoadedPlayerCount++;
			}
		}
	}

	// 4. 약속된 인원 모두가 100% 로딩을 완료했는가?
	if (FullyLoadedPlayerCount == ExpectedPlayers)
	{
		LOG_CJH(Log, TEXT("[GameLoop] 로비에서 넘어온 %d명 전원 로딩 완료! Init 상태 진입."), ExpectedPlayers);
		SetMatchState(DDGameplayTags::State_BoardGame_Init);
	}
}

void ADDBoardGameMode::TickState_PlayerTurn()
{
	if (CachedBoardGameState->GetStateTimer() > 0)
	{
		LOG_CJH(Log, TEXT("현재 턴 남은 시간: %d초"), CachedBoardGameState->GetStateTimer());
		CachedBoardGameState->DecreaseStateTimer();
		
		if (CachedBoardGameState->GetStateTimer() == 0)
		{
			int32 CurrentIndex = CachedBoardGameState->GetTurnPlayerIndex();
			if (AlivePlayerControllers.IsValidIndex(CurrentIndex))
			{
			  if (ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(AlivePlayerControllers[CurrentIndex]))
			  {
			      DDPC->Client_ClosePopUp(DDGameplayTags::BoardGame_UI_PlayerTurn);
			  }
			}
			
			ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(AlivePlayerControllers[CachedBoardGameState->GetTurnPlayerIndex()]);
			if (DDPC)
			{
				// 1. UI 팝업 닫기
				DDPC->Client_ClosePopUp(DDGameplayTags::BoardGame_UI_PlayerTurn);

				// 2. 현재 턴이었던 플레이어의 실행 중인 어빌리티 강제 종료
				if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromPlayer(DDPC))
				{
					ASC->CancelAllAbilities();
				}
			}
			
			LOG_CJH(Log, TEXT("[TimeOut] 턴 제한 시간 초과! 다음 플레이어로 강제 전환."));
			SetMatchState(DDGameplayTags::State_BoardGame_PlayerTurn);
		}
	}
}

void ADDBoardGameMode::TickState_RoundEnd()
{
	if (CachedBoardGameState->GetStateTimer() > 0)
	{
		LOG_CJH(Log, TEXT("[Travel] 진입 전 시간: %d초"), CachedBoardGameState->GetStateTimer());
		CachedBoardGameState->DecreaseStateTimer();
		
		if (CachedBoardGameState->GetStateTimer() == 0)
		{
			TArray<APlayerState*> MiniGamePlayers;
			for (APlayerController* PC : AlivePlayerControllers)
			{
				if (IsValid(PC) && PC->PlayerState)
				{
					MiniGamePlayers.Add(PC->PlayerState);
				}
				
				if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromPlayer(PC))
	            {
	                ASC->CancelAllAbilities();
	            }
			}
			
			LOG_CJH(Log, TEXT("[Travel] 미니게임 참여 인원: %d명"), AlivePlayerControllers.Num());
			
			for (APlayerState* PS : GameState->PlayerArray)
		    {
		        if (ADDBasePlayerState* DDPS = Cast<ADDBasePlayerState>(PS))
		        {
		            DDPS->CurrentTile = nullptr; // 트래블 전에 연결고리 끊기
		        }
		    }
			
			if (UDDMiniGameManager* MiniGameManager = GetGameInstance()->GetSubsystem<UDDMiniGameManager>())
			{
				LOG_CJH(Log, TEXT("[Travel] 미니게임에 진입합니다."));
				MiniGameManager->RequestStartRandomMiniGame(MiniGamePlayers);
			}
		}
	}
}

void ADDBoardGameMode::SetMatchState(FGameplayTag NewStateTag)
{
	if (!IsValid(CachedBoardGameState)) return;
	
	LOG_CJH(Log, TEXT("=== [상태 전환] 새로운 상태: %s ==="), *NewStateTag.ToString());
	
	// 1. GameState 상태값 갱신
	CachedBoardGameState->SetBoardMatchState(NewStateTag);

	// 2. 서버 측 공통 이펙트 처리
	ApplyMatchStateEffects(NewStateTag);

	if (NewStateTag == DDGameplayTags::State_BoardGame_Init)
	{
		HandleState_Init();
	}
	else if (NewStateTag == DDGameplayTags::State_BoardGame_PlayerTurn)
	{
		BroadcastGameLayer(DDGameplayTags::State_BoardGame_Init);
		StartNextPlayerTurn();
	}
	else if (NewStateTag == DDGameplayTags::State_BoardGame_RoundEnd)
	{
		HandleState_RoundEnd();
	}
	else if (NewStateTag == DDGameplayTags::State_BoardGame_Ending)
	{
		CalculateFinalWinner();
	}
	else if (NewStateTag == DDGameplayTags::State_BoardGame_End)
	{
		HandleState_End();
	}
}

void ADDBoardGameMode::HandleState_Init()
{
	SortPlayersByTurnOrder();
	LOG_CYS(Warning, TEXT("[GM] 보드게임 초기화 시작"));
	
	// 타일 매니저 초기화
	if (!IsValid(CachedBoardGameState->TileManager))
	{
		LOG_CYS(Error,TEXT("[GM] 타일 매니저 못 찾음"));
	}
	CachedBoardGameState->TileManager->InitializeTiles();

	for (APlayerController* PC : AlivePlayerControllers)
	{
		ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(PC);
		if (!DDPC) continue;
		
		if (ADDBasePlayerState* PS = DDPC->GetCachedPlayerState()) PS->InitTile();
		
		
		if (!DDPC->GetPawn()) RestartPlayer(DDPC);
		if (auto Char = Cast<ADDBoardGameCharacter>(DDPC->GetPawn())) Char->InitLocation();
		
		DDPC->Client_ApplyState(DDGameplayTags::State_BoardGame_Init);
		
	}
	
	LOG_CYS(Warning, TEXT("[GM] 보드게임 전체 초기화 완료"));
	
	UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance());
	if (IsValid(GameInstance) && GameInstance->CurrentRound == 0)
	{
		LOG_CYS(Warning, TEXT("1라운드 시퀀서 재생 지시"));
		
		if (CachedBoardGameState)
		{
			CachedBoardGameState->Multicast_PlaySequence();
		}
	}
	else
	{
		if (CachedBoardGameState)
		{
			CachedBoardGameState->Multicast_PlayBoardBGM();
		}
		CheckWinCondition();
	}
}

void ADDBoardGameMode::HandleState_RoundEnd()
{
	// 1. 승리 조건 선제 검사
	for (APlayerController* PC : AlivePlayerControllers)
	{
		ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(PC);
		if (DDPC && DDPC->GetCachedPlayerState())
		{
			UDDPointSet* PointSet = DDPC->GetCachedPlayerState()->GetPointSet();
			if (PointSet && PointSet->GetTrophy() >= CachedBoardGameState->MaxTrophy)
			{
				LOG_CJH(Log, TEXT("[승자 발생] %s 트로피 달성!"), *DDPC->GetCachedPlayerState()->GetPlayerDisplayName().ToString());
				SetMatchState(DDGameplayTags::State_BoardGame_Ending);
				return;
			}
		}
	}
   
	// 2. 승자 없으면 대기 시간 설정 및 타일 정보 저장
	CachedBoardGameState->SetStateTimer(3);
   
	for (APlayerController* PC : AlivePlayerControllers)
	{
		if (ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(PC))
		{
			ADDBasePlayerState* PS = DDPC->GetCachedPlayerState();
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

void ADDBoardGameMode::HandleState_End()
{
	LOG_CJH(Log, TEXT("게임 종료. 10초 후 로비 이동."));
	BroadcastErrorMessage(TEXT("게임 종료. 10초 후 로비 이동.")); 
	
	GetWorldTimerManager().SetTimer(
		ReturnToLobbyTimerHandle,
		this,
		&ThisClass::TravelToLobby,
		10.0f,
		false
	);
}

void ADDBoardGameMode::StartNextPlayerTurn()
{
	int32 NextIndex = CachedBoardGameState->GetTurnPlayerIndex() + 1;

	// 라운드 종료 체크
	if (NextIndex >= AlivePlayerControllers.Num())
	{
		ProcessRoundTransition();
		return;
	}

	// 데이터 갱신
	CachedBoardGameState->SetTurnPlayerIndex(NextIndex);
	CachedBoardGameState->SetStateTimer(CachedBoardGameState->MaxStateTimer);

	// 서버 측 이펙트 적용
	ApplyTurnEffectsToPlayers();
	SetTurnPhase(DDGameplayTags::State_TurnPhase_BeforeDice);
	
	
	// PC들에게 반응 지시
	for (int32 i = 0; i < AlivePlayerControllers.Num(); ++i)
	{
		ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(AlivePlayerControllers[i]);
		if (!DDPC) continue;

		if (i == NextIndex)
		{
			DDPC->Client_SetMouseCursorVisible(true);
			DDPC->Client_OpenPopUp(DDGameplayTags::BoardGame_UI_PlayerTurn); 
		}
		else
		{
			// DDPC->Client_SetMouseCursorVisible(false); 턴 넘어가도 커서 안숨김
		}
	}

	APawn* ActivePawn = AlivePlayerControllers[NextIndex]->GetPawn();
	Super::FocusAllCamerasOnTarget(ActivePawn);
}

void ADDBoardGameMode::ProcessRoundTransition()
{
	CachedBoardGameState->SetTurnPlayerIndex(-1); // 턴 초기화
	
	for (APlayerController* PC : AlivePlayerControllers)
    {
       if (ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(PC))
       {
          if (ADDBasePlayerState* PS = DDPC->GetCachedPlayerState())
          {
             PS->SetTurnOrder(-1);
          }
       }
    }
	
	if (UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance()))
	{
		GameInstance->CurrentRound++;
		CachedBoardGameState->SetCurrentRound(CachedBoardGameState->MaxRound - GameInstance->CurrentRound);
	}
	LOG_CJH(Log, TEXT("모든 플레이어의 턴 종료. 라운드를 넘깁니다."));
	SetMatchState(DDGameplayTags::State_BoardGame_RoundEnd);
}

void ADDBoardGameMode::CheckWinCondition()
{
	LOG_CJH(Log, TEXT("[CheckWinCondition] 승리 및 라운드 도달 검사"));
	UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance());

	if (!IsValid(CachedBoardGameState) || !IsValid(GameInstance)) return;

	bool bHasTrophyWinner = false;
	for (APlayerState* PS : CachedBoardGameState->PlayerArray)
	{
		ADDBasePlayerState* BasePS = Cast<ADDBasePlayerState>(PS);
		if (BasePS && BasePS->GetPointSet() && BasePS->GetPointSet()->GetTrophy() >= CachedBoardGameState->MaxTrophy)
		{
			LOG_CJH(Log, TEXT("목표 트로피 도달자 발생! (닉네임: %s)"),
				        *BasePS->PlayerGameData.PlayerDisplayName.ToString());
			bHasTrophyWinner = true;
			break;
		}
	}

	if (bHasTrophyWinner || GameInstance->CurrentRound > CachedBoardGameState->MaxRound)
	{
		LOG_CJH(Log, TEXT("[CheckWinCondition] 게임 종료 조건 충족. Ending 페이즈로 전환합니다."));
		SetMatchState(DDGameplayTags::State_BoardGame_Ending);
	}
	else
	{
		LOG_CJH(Log, TEXT("[CheckWinCondition] 게임을 계속 진행합니다. (현재 라운드: %d)"), GameInstance->CurrentRound + 1);
		CachedBoardGameState->SetTurnPlayerIndex(-1);
		SetMatchState(DDGameplayTags::State_BoardGame_PlayerTurn);
	}
}

void ADDBoardGameMode::CalculateFinalWinner()
{
	if (!IsValid(CachedBoardGameState)) return;

    // GameState에 구현된 '공통 정렬 로직'을 호출하여 이미 정렬된 배열을 받아옵니다.
    TArray<ADDBasePlayerState*> SortedPlayers = CachedBoardGameState->UpdateAndGetPlayerRanks();

    TArray<FFinalRankData> FinalResults;
    for (ADDBasePlayerState* PS : SortedPlayers)
    {
		PS->SetIsGameFinished(true);
		
		FFinalRankData Data;
        // 이미 갱신된 순위(CurrentRank)를 그대로 사용
        Data.Rank = PS->PlayerGameData.CurrentRank;
        Data.PlayerName = PS->GetPlayerDisplayName();
        Data.Trophy = (int32)PS->GetPointSet()->GetTrophy();
        Data.Coin = (int32)PS->GetPointSet()->GetCoin();
    	Data.PlayerColor = PS->PlayerGameData.PlayerColor;
        FinalResults.Add(Data);
       
		LOG_CJH(Log, TEXT("%d등 플레이어: %s | 트로피 개수: %d, 코인 개수: %d"),
          Data.Rank,
          *Data.PlayerName.ToString(),
          Data.Trophy,
          Data.Coin
		);
    }
	
	UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance());  
    if (GameInstance)  
    {        
	    CachedBoardGameState->SetCurrentRound(0);  
    }    
    
    CachedBoardGameState->SetFinalRankings(FinalResults);
    SetMatchState(DDGameplayTags::State_BoardGame_End);
	
	BroadcastOpenPopUp(DDGameplayTags::BoardGame_UI_Result);
}

void ADDBoardGameMode::RequestRankUpdate()
{
    if (!GetWorldTimerManager().IsTimerActive(RankUpdateTimerHandle))
    {
        GetWorldTimerManager().SetTimer(
            RankUpdateTimerHandle,
            this,
            &ThisClass::ExecuteRankUpdate,
            0.1f,
            false
        );
    }
}

void ADDBoardGameMode::ExecuteRankUpdate()
{
    if (CachedBoardGameState)
    {
        CachedBoardGameState->UpdateAndGetPlayerRanks();
    }
}

void ADDBoardGameMode::ApplyMatchStateEffects(FGameplayTag NewStateTag)
{
	for (APlayerController* PC : AlivePlayerControllers)
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromPlayer(PC);
		if (!ASC) continue;

		ASC->RemoveActiveEffectsWithGrantedTags(CurrentAppliedTags);

		if (TSubclassOf<UGameplayEffect>* EffectClassPtr = MatchStateEffectClasses.Find(NewStateTag))
		{
			if (*EffectClassPtr)
			{
				FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
				Context.AddSourceObject(this);
				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(*EffectClassPtr, 1.0f, Context);
				if (SpecHandle.IsValid()) ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
	CurrentAppliedTags = FGameplayTagContainer(NewStateTag);
}

void ADDBoardGameMode::ApplyTurnEffectsToPlayers()
{
	for (int32 i = 0; i < AlivePlayerControllers.Num(); ++i)
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromPlayer(AlivePlayerControllers[i]);
		if (!ASC) continue;

		FGameplayTagContainer TurnTagsToRemove;
		TurnTagsToRemove.AddTag(DDGameplayTags::State_BoardGame_TurnActive);
		TurnTagsToRemove.AddTag(DDGameplayTags::State_BoardGame_TurnWaiting);
		TurnTagsToRemove.AddTag(DDGameplayTags::State_BoardGame_HasUsedItem);
		ASC->RemoveActiveEffectsWithGrantedTags(TurnTagsToRemove);

		TSubclassOf<UGameplayEffect> EffectClassPtr = (i == CachedBoardGameState->GetTurnPlayerIndex()) ? TurnActiveEffectClass : TurnWaitingEffectClass;
		if (EffectClassPtr)
		{
			FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
			Context.AddSourceObject(this);
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClassPtr, 1.0f, Context);
			if (SpecHandle.IsValid()) ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void ADDBoardGameMode::Logout(AController* Exiting)
{
	APlayerController* ExitingPC = Cast<APlayerController>(Exiting);
	if (!IsValid(ExitingPC)) return;

	int32 ExitingIndex = AlivePlayerControllers.Find(ExitingPC);
	bool bWasCurrentTurn = (CachedBoardGameState && ExitingIndex == CachedBoardGameState->GetTurnPlayerIndex());
	AlivePlayerControllers.Remove(ExitingPC);

	if (CachedBoardGameState && AlivePlayerControllers.Num() < CachedBoardGameState->GetMinPlayerCount())
	{
		FGameplayTag CurrentState = CachedBoardGameState->GetBoardMatchState();

		// 1-1. 대기실 상태였다면: 조기 종료할 필요 없이 자연스럽게 틱(Tick)에서 대기 상태 유지
		if (!CurrentState.IsValid())
		{
			Super::Logout(Exiting);
			return;
		}

		// 1-2. 이미 정상적인 종료 과정 중이라면 중복 실행 방지
		if (CurrentState == DDGameplayTags::State_BoardGame_Ending || CurrentState == DDGameplayTags::State_BoardGame_End)
		{
			Super::Logout(Exiting);
			return;
		}

		// 1-3. 게임 진행 중 인원 미달: 조기 종료(기권 승) 후 결과창으로 즉시 이동!
		LOG_CJH(Warning, TEXT("진행 중 인원 미달 발생! 남은 인원으로 조기 종료 후 결과창을 표시합니다."));
		
		GetWorldTimerManager().ClearTimer(TurnTransitionTimerHandle); // 혹시 돌고 있을 턴 딜레이 취소

		// 남은 플레이어들에게 안내
		for (APlayerController* PC : AlivePlayerControllers)
		{
			if (ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(PC))
			{
				DDPC->Client_DrawErrorMessage(TEXT("상대방의 이탈로 게임이 조기 종료되었습니다."), 3.0f);
			}
		}

		// CalculateFinalWinner를 호출하게 만들어 남은 사람들끼리 정렬하여 1등을 부여함
		SetMatchState(DDGameplayTags::State_BoardGame_Ending); 
		Super::Logout(Exiting);
		return;
	}
	
	if (bWasCurrentTurn)
	{
		GetWorldTimerManager().ClearTimer(TurnTransitionTimerHandle);
		if (CachedBoardGameState->GetTurnPlayerIndex() >= AlivePlayerControllers.Num())
		{
			ProcessRoundTransition();
		}
		else
		{
			// 턴 보정 후 재생성
			CachedBoardGameState->SetTurnPlayerIndex(CachedBoardGameState->GetTurnPlayerIndex() - 1);
			SetMatchState(DDGameplayTags::State_BoardGame_PlayerTurn);
		}
	}
	Super::Logout(Exiting);
}

void ADDBoardGameMode::NotifyDiceRolled()
{
	LOG_CJH(Log, TEXT("[Notify] 주사위 굴림. BeforeDice를 제거하고 Moving 페이즈로 전환."));
	SetTurnPhase(DDGameplayTags::State_TurnPhase_Moving);
	CachedBoardGameState->SetStateTimer(-1);
}

void ADDBoardGameMode::NotifyMovementFinished()
{
	LOG_CJH(Log, TEXT("[Notify] 이동 완료. Moving 태그를 제거하고 %.0f초 뒤 턴을 넘깁니다."), TurnTransitionTimer);
	SetTurnPhase(FGameplayTag::EmptyTag);
	
	GetWorldTimerManager().SetTimer(
		TurnTransitionTimerHandle, 
		this, 
		&ThisClass::ExecuteNextTurnTransition,
		TurnTransitionTimer,
		false
	);
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
	LOG_CYS(Error,TEXT("리스폰 성공"));
	
	// 풀피
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(PS);
	if (!ASCInterface) return;
	
	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();

	if (!ASC) return;

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(HealingEffect, 1.f, ContextHandle);

	if (!SpecHandle.IsValid())
	{
		return;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(
		DDGameplayTags::Data_Health_Heal,
		30.0f
	);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}

void ADDBoardGameMode::ExecuteNextTurnTransition()
{
	LOG_CJH(Log, TEXT("[Timer] %.0f초 대기 완료. 다음 플레이어로 턴을 전환합니다."), TurnTransitionTimer);
	if (!AlivePlayerControllers.IsValidIndex(CachedBoardGameState->GetTurnPlayerIndex())) return;
	
	ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(AlivePlayerControllers[CachedBoardGameState->GetTurnPlayerIndex()]);
	if (DDPC)
	{
		// 1. UI 팝업 닫기
		DDPC->Client_ClosePopUp(DDGameplayTags::BoardGame_UI_PlayerTurn);

		// 2. 현재 턴이었던 플레이어의 실행 중인 어빌리티 강제 종료
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromPlayer(DDPC))
		{
			ASC->CancelAllAbilities();
		}
		
		SetMatchState(DDGameplayTags::State_BoardGame_PlayerTurn);
	}
}

void ADDBoardGameMode::TravelToLobby()
{
	if (GetWorld())
    {
        LOG_CJH(Log, TEXT("로비로 ServerTravel을 시작합니다."));
        GetWorld()->ServerTravel(LobbyMapPath);
    }
}

void ADDBoardGameMode::HandleInventoryOpenRequest(ADDBasePlayerController* Requester)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromPlayer(Requester);
	if (!ASC || !ASC->HasMatchingGameplayTag(DDGameplayTags::State_BoardGame_TurnActive)) return; 

	BroadcastOpenPopUp(DDGameplayTags::BoardGame_UI_Inventory);
}

void ADDBoardGameMode::HandleInventoryClose()
{
	LOG_KMS(Warning, TEXT("인벤토리 닫기 실행"));
	BroadcastClosePopUp(DDGameplayTags::BoardGame_UI_Inventory); 
}

void ADDBoardGameMode::SetTurnPhase(FGameplayTag NewPhaseTag)
{
	int32 CurrentIndex = CachedBoardGameState->GetTurnPlayerIndex();
	if (!AlivePlayerControllers.IsValidIndex(CurrentIndex)) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromPlayer(AlivePlayerControllers[CurrentIndex]);
	if (!ASC) return;

	if (CurrentTurnPhaseTag.IsValid()) ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(CurrentTurnPhaseTag));

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
				if (SpecHandle.IsValid()) ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
	
	LOG_CJH(Log, TEXT("   └ [TurnPhase] 페이즈 갱신: %s"), *NewPhaseTag.ToString());
}

void ADDBoardGameMode::SortPlayersByTurnOrder()
{
	for (int32 i = 0; i < AlivePlayerControllers.Num(); ++i)
	{
		if (ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(AlivePlayerControllers[i]))
		{
			ADDBasePlayerState* PS = DDPC->GetCachedPlayerState();
			if (PS && PS->GetTurnOrder() < 0) PS->SetTurnOrder(i + 100);
		}
	}

	AlivePlayerControllers.Sort([](const APlayerController& A, const APlayerController& B)
{
    // 1. 역참조된 객체의 주소를 가져와 포인터로 변환
    const ADDBasePlayerController* PCA = Cast<ADDBasePlayerController>(&A);
    const ADDBasePlayerController* PCB = Cast<ADDBasePlayerController>(&B);

    // 2. 유효성 검사 및 정렬 기준 비교
    if (PCA && PCB)
    {
        const ADDBasePlayerState* PSA = PCA->GetCachedPlayerState();
        const ADDBasePlayerState* PSB = PCB->GetCachedPlayerState();

        if (PSA && PSB)
        {
            return PSA->GetTurnOrder() < PSB->GetTurnOrder();
        }
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
			        *BasePlayerState->GetPlayerDisplayName().ToString(),
			        BasePlayerState->GetTurnOrder());
		}
	}
	LOG_CJH(Log, TEXT("====================================="));
}
