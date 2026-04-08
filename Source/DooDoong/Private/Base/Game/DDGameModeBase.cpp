#include "Base/Game/DDGameModeBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "EngineUtils.h"
#include "Base/Game/DDGameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpectatorPawn.h"
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
#include "InputMappingContext.h"

ADDGameModeBase::ADDGameModeBase()
{
	PlayerStateClass = ADDBasePlayerState::StaticClass();
	PlayerControllerClass = ADDBasePlayerController::StaticClass();

	bUseSeamlessTravel = true;
}

void ADDGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	LOG_CJH(Log, TEXT("[BeginPlay] 보드게임 맵 진입 완료. 메인 타이머 시작."));

	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.f, true);
}

void ADDGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ADDGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerController* ExitingPlayerController = Cast<APlayerController>(Exiting);
	if (IsValid(ExitingPlayerController))
	{
		AlivePlayerControllers.Remove(ExitingPlayerController);
	}
}

void ADDGameModeBase::HandleSeamlessTravelPlayer(AController*& ParticipantController)
{
	Super::HandleSeamlessTravelPlayer(ParticipantController);

	// 심리스 트래블 시 이전 맵에서 넘어온 찌꺼기 폰 제거
	if (APawn* OldPawn = ParticipantController->GetPawn())
	{
		OldPawn->Destroy();
	}

	APlayerController* PlayerController = Cast<APlayerController>(ParticipantController);

	// 복사된 PlayerState의 정보를 확인하여 관전자와 참여자를 분리
	if (ADDBasePlayerState* BasePlayerState = ParticipantController->GetPlayerState<ADDBasePlayerState>())
	{
		LOG_CJH(Log, TEXT("[SeamlessTravel] 접속 유저: %s, 참여 여부: %d"), *BasePlayerState->PlayerGameData.PlayerDisplayName.ToString(),
		       BasePlayerState->bIsParticipant);

		if (!BasePlayerState->bIsParticipant)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			APawn* Spectator = GetWorld()->SpawnActor<ASpectatorPawn>(
				ASpectatorPawn::StaticClass(), FVector(0, 0, 1000), FRotator::ZeroRotator, SpawnParams);

			if (Spectator) ParticipantController->Possess(Spectator);
			if (IsValid(PlayerController)) AlivePlayerControllers.Remove(PlayerController);

			return;
		}
	}

	// 실제 참여자인 경우 캐릭터를 스폰하고 턴 명단에 등록
	RestartPlayer(ParticipantController);

	if (IsValid(PlayerController))
	{
		AlivePlayerControllers.AddUnique(PlayerController);
	}
	LOG_CJH(Log, TEXT("-> 참여자 스폰 완료! 현재 턴 명단 인원: %d명"), AlivePlayerControllers.Num());
}

void ADDGameModeBase::GenericPlayerInitialization(AController* C)
{
	Super::GenericPlayerInitialization(C);

	APlayerController* PlayerController = Cast<APlayerController>(C);

	if (IsValid(PlayerController))
	{
		AlivePlayerControllers.AddUnique(PlayerController);
		LOG_CJH(Log, TEXT("[GenericPlayerInitialization] 참여자 접속. 현재 접속 수: %d"), AlivePlayerControllers.Num());
	}
	
}

void ADDGameModeBase::OnMainTimerElapsed()
{
	ADDGameStateBase* GameStateBase = GetGameState<ADDGameStateBase>();
	if (!IsValid(GameStateBase)) return;

	// 1. 게임 최초 시작 대기 로직
	if (!GameStateBase->MatchStateTag.IsValid())
	{
		if (AlivePlayerControllers.Num() >= GameStateBase->MinPlayerCount)
		{
			LOG_CJH(Log, TEXT("[GameLoop] %d명 접속 완료! 보드게임 Init 상태로 진입합니다."), GameStateBase->MinPlayerCount);
			SetMatchState(DDGameplayTags::State_BoardGame_Init);
		}
		return;
	}

	// 2. 플레이어 턴 제한 시간 관리
	if (GameStateBase->MatchStateTag == DDGameplayTags::State_BoardGame_PlayerTurn)
	{
		if (StateTimer > 0)
		{
			StateTimer--;
			LOG_CJH(Log, TEXT("현재 턴 남은 시간: %d"), StateTimer);

			if (StateTimer == 0)
			{
				LOG_CJH(Log, TEXT("[TimeOut] 턴 제한 시간 초과! 다음 플레이어로 턴을 강제 전환합니다."));
				CurrentTurnPlayerIndex++;
				SetMatchState(DDGameplayTags::State_BoardGame_PlayerTurn);
			}
		}
	}
	// 3. 미니게임 이동 대기 시간 관리
	else if (GameStateBase->MatchStateTag == DDGameplayTags::State_BoardGame_RoundEnd)
	{
		if (StateTimer > 0)
		{
			StateTimer--;
			if (StateTimer == 0)
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
					MiniGameManager->RequestStartRandomMiniGame(MiniGamePlayers);
				}
			}
		}
	}
}

void ADDGameModeBase::SetMatchState(FGameplayTag NewStateTag)
{
	ADDGameStateBase* GameStateBase = GetGameState<ADDGameStateBase>();
	if (!IsValid(GameStateBase)) return;

	LOG_CJH(Log, TEXT("================================================="));
	LOG_CJH(Log, TEXT("🔄 [상태 전환] 새로운 상태: %s"), *NewStateTag.ToString());
	LOG_CJH(Log, TEXT("================================================="));

	GameStateBase->MatchStateTag = NewStateTag;

	FGameplayTagContainer TagsToApply;
	TagsToApply.AddTag(NewStateTag);

	// 기획자가 설정한 부가 태그 병합
	if (const FGameplayTagContainer* EditorTags = StateTagMapping.Find(NewStateTag))
	{
		TagsToApply.AppendTags(*EditorTags);
	}

	// 모든 플레이어에게 이전 태그를 제거하고 새 태그 부여
	for (APlayerController* PlayerController : AlivePlayerControllers)
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromPlayer(PlayerController))
		{
			AbilitySystemComponent->RemoveLooseGameplayTags(CurrentAppliedTags);
			AbilitySystemComponent->RemoveReplicatedLooseGameplayTags(CurrentAppliedTags);
			AbilitySystemComponent->AddLooseGameplayTags(TagsToApply);
			AbilitySystemComponent->AddReplicatedLooseGameplayTags(TagsToApply);
			
		}
	}
	CurrentAppliedTags = TagsToApply;

	// 각 상태에 따른 게임 루프 실행
	if (NewStateTag == DDGameplayTags::State_BoardGame_Init)
	{
		SortPlayersByTurnOrder();
		
		LOG_CYS(Warning, TEXT("[GM] 보드게임 전체 초기화 시작"));

		// 1️⃣ TileManager 초기화 (1번만)
		ADDTileManager* TM = nullptr;
		for (TActorIterator<ADDTileManager> It(GetWorld()); It; ++It)
		{
			TM = *It;
			break;
		}

		if (TM)
		{
			TM->InitializeTiles();
		}

		// 2️⃣ PlayerState InitTile
		for (APlayerController* PC : AlivePlayerControllers)
		{
			if (auto PS = PC->GetPlayerState<ADDBasePlayerState>())
			{
				PS->InitTile();
			}
		}

		// 3️⃣ Character InitLocation
		for (APlayerController* PC : AlivePlayerControllers)
		{
			// 혹시 모를 안전장치
			if (!PC->GetPawn())
			{
				RestartPlayer(PC);
			}

			if (auto Char = Cast<ADDBoardGameCharacter>(PC->GetPawn()))
			{
				Char->InitLocation();
			}
			// IMC 교체
			if (auto DDPC = Cast<ADDBasePlayerController>(PC))
			{
				DDPC->Client_ApplyState(NewStateTag);
			}
		}

		LOG_CYS(Warning, TEXT("[GM] 보드게임 전체 초기화 완료"));

		CheckWinCondition();
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
		StateTimer = 3;
		LOG_CJH(Log, TEXT("⏱️ [라운드 종료] 3초 뒤 미니게임으로 이동합니다."));
	}
}

void ADDGameModeBase::CheckWinCondition()
{
	LOG_CJH(Log, TEXT("[CheckWinCondition] 승리 조건 및 최대 라운드 도달 여부를 검사합니다."));

	ADDGameStateBase* GameStateBase = GetGameState<ADDGameStateBase>();
	UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance());

	if (!IsValid(GameStateBase) || !IsValid(GameInstance)) return;

	bool bHasTrophyWinner = false;

	// 모든 접속자의 포인트셋(트로피) 확인
	for (APlayerState* PlayerState : GameStateBase->PlayerArray)
	{
		if (ADDBasePlayerState* BasePlayerState = Cast<ADDBasePlayerState>(PlayerState))
		{
			if (BasePlayerState->GetPointSet() && BasePlayerState->GetPointSet()->GetTrophy() >= MaxTrophy)
			{
				LOG_CJH(Log, TEXT("🏆 목표 트로피 도달자 발생! (닉네임: %s)"), *BasePlayerState->PlayerGameData.PlayerDisplayName.ToString());
				bHasTrophyWinner = true;
				break;
			}
		}
	}

	if (bHasTrophyWinner || GameInstance->CurrentRound > MaxRound)
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

void ADDGameModeBase::StartNextPlayerTurn()
{
	if (CurrentTurnPlayerIndex >= AlivePlayerControllers.Num())
	{
		CurrentTurnPlayerIndex = 0;

		UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance());
		if (IsValid(GameInstance))
		{
			GameInstance->CurrentRound++;
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
		ADDBasePlayerController* BasePlayerController = Cast<ADDBasePlayerController>(PlayerController);

		if (!IsValid(BasePlayerController)) continue;

		UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromPlayer(BasePlayerController);
		if (!AbilitySystemComponent) continue;

		if (i == CurrentTurnPlayerIndex)
		{
			// 현재 턴 플레이어에게 권한 태그 부여
			AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(DDGameplayTags::State_BoardGame_TurnWaiting);
			AbilitySystemComponent->RemoveLooseGameplayTag(DDGameplayTags::State_BoardGame_TurnWaiting);
			AbilitySystemComponent->SetLooseGameplayTagCount(DDGameplayTags::State_BoardGame_TurnWaiting, 0);
			AbilitySystemComponent->SetReplicatedLooseGameplayTagCount(DDGameplayTags::State_BoardGame_TurnWaiting, 0);
			AbilitySystemComponent->AddReplicatedLooseGameplayTag(DDGameplayTags::State_BoardGame_TurnActive);
			AbilitySystemComponent->AddLooseGameplayTag(DDGameplayTags::State_BoardGame_TurnActive);
			AbilitySystemComponent->SetLooseGameplayTagCount(DDGameplayTags::State_BoardGame_TurnActive, 1);
			AbilitySystemComponent->SetReplicatedLooseGameplayTagCount(DDGameplayTags::State_BoardGame_TurnActive, 1);

			StateTimer = MaxStateTimer;

			LOG_CJH(Log, TEXT("▶ [%d]번 플레이어 턴 시작! (제한시간 %d초)"), i, MaxStateTimer);

			SetTurnPhase(DDGameplayTags::State_TurnPhase_BeforeDice);
		}
		else
		{
			// 타인에게 대기 태그 부여
			AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(DDGameplayTags::State_BoardGame_TurnActive);
			AbilitySystemComponent->RemoveLooseGameplayTag(DDGameplayTags::State_BoardGame_TurnActive);
			AbilitySystemComponent->SetLooseGameplayTagCount(DDGameplayTags::State_BoardGame_TurnActive, 0);
			AbilitySystemComponent->SetReplicatedLooseGameplayTagCount(DDGameplayTags::State_BoardGame_TurnActive, 0);
			AbilitySystemComponent->AddReplicatedLooseGameplayTag(DDGameplayTags::State_BoardGame_TurnWaiting);
			AbilitySystemComponent->AddLooseGameplayTag(DDGameplayTags::State_BoardGame_TurnWaiting);
			AbilitySystemComponent->SetLooseGameplayTagCount(DDGameplayTags::State_BoardGame_TurnWaiting, 1);
			AbilitySystemComponent->SetReplicatedLooseGameplayTagCount(DDGameplayTags::State_BoardGame_TurnWaiting, 1);
		}
	}

	// 현재 턴 플레이어 화면으로 모두의 카메라 시점 이동
	FocusAllCamerasOnTarget(ActivePawn);
}

void ADDGameModeBase::NotifyDiceRolled()
{
	LOG_CJH(Log, TEXT("[Notify] 주사위 굴림 감지. 타이머를 멈추고 이동 페이즈로 전환합니다."));
	StateTimer = -1;
	SetTurnPhase(DDGameplayTags::State_TurnPhase_Moving);
}

void ADDGameModeBase::NotifyMovementFinished()
{
	LOG_CJH(Log, TEXT("[Notify] 캐릭터 이동 완료. 발판 이벤트 페이즈로 전환합니다."));
	SetTurnPhase(DDGameplayTags::State_TurnPhase_Event);
}

void ADDGameModeBase::NotifyTileEventFinished()
{
	LOG_CJH(Log, TEXT("[Notify] 발판 이벤트 완료. 턴을 종료하고 다음 플레이어로 넘어갑니다."));

	SetTurnPhase(FGameplayTag::EmptyTag);
	CurrentTurnPlayerIndex++;
	SetMatchState(DDGameplayTags::State_BoardGame_PlayerTurn);
}

void ADDGameModeBase::SetTurnPhase(FGameplayTag NewPhaseTag)
{
	if (!AlivePlayerControllers.IsValidIndex(CurrentTurnPlayerIndex)) return;

	APlayerController* PlayerController = AlivePlayerControllers[CurrentTurnPlayerIndex];
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromPlayer(PlayerController);
	if (!AbilitySystemComponent) return;

	if (CurrentTurnPhaseTag.IsValid())
	{
		AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(CurrentTurnPhaseTag);
		AbilitySystemComponent->RemoveLooseGameplayTag(CurrentTurnPhaseTag);
		AbilitySystemComponent->SetLooseGameplayTagCount(CurrentTurnPhaseTag, 0);
		AbilitySystemComponent->SetReplicatedLooseGameplayTagCount(CurrentTurnPhaseTag, 0);
		
	}

	CurrentTurnPhaseTag = NewPhaseTag;

	if (CurrentTurnPhaseTag.IsValid())
	{
		AbilitySystemComponent->AddReplicatedLooseGameplayTag(CurrentTurnPhaseTag);
		AbilitySystemComponent->AddLooseGameplayTag(CurrentTurnPhaseTag);
		AbilitySystemComponent->SetLooseGameplayTagCount(CurrentTurnPhaseTag, 1);
		AbilitySystemComponent->SetReplicatedLooseGameplayTagCount(CurrentTurnPhaseTag, 1);
		LOG_CJH(Log, TEXT("   └ [TurnPhase] 페이즈 갱신: %s"), *NewPhaseTag.ToString());
	}
}

void ADDGameModeBase::FocusAllCamerasOnTarget(AActor* TargetActor)
{
	if (!IsValid(TargetActor)) return;

	for (APlayerController* PlayerController : AlivePlayerControllers)
	{
		if (IsValid(PlayerController))
		{
			PlayerController->SetViewTargetWithBlend(TargetActor, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
		}
	}
	LOG_CJH(Log, TEXT("[Camera] 모든 플레이어의 화면이 주인공 타겟(%s)을 향합니다."), *TargetActor->GetName());
}

void ADDGameModeBase::OnCharacterKilled(AActor* Killer, AActor* Victim)
{
	LOG_KMS(Warning, TEXT("[GM] : %s was killed by %s"), *Victim->GetName(), *Killer->GetName());
}


void ADDGameModeBase::HandleRespawn(AController* TargetController)
{
	if (TargetController)
	{
		// 1. 기존 캐릭터 제거  
		if (APawn* OldPawn = TargetController->GetPawn())
		{
			OldPawn->Destroy();
		}
		
		// 2. ASC 초기화
		ADDBasePlayerState* PS = TargetController->GetPlayerState<ADDBasePlayerState>();
		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent(); 
		
		ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(DDGameplayTags::State_Character_Death));
		ASC->RemoveLooseGameplayTag(DDGameplayTags::State_Character_Death);
		ASC->SetLooseGameplayTagCount(DDGameplayTags::State_Character_Death, 0);
		
		for (auto EffectClass : ReSpawnEffectClasses)
		{
			if (IsValid(EffectClass))
			{
				FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
				Context.AddSourceObject(ASC);
				
				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, 1.f, Context);
				if (SpecHandle.IsValid())
				{
					FActiveGameplayEffectHandle ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		}
		
		// 3. 새로운 캐릭터 생성 및 할당 
		RestartPlayer(TargetController);
		LOG_KMS(Warning, TEXT("[GameMode] %s : ReSpawned."), *TargetController->GetPawn()->GetName());
	}
}

void ADDGameModeBase::CalculateFinalWinner()
{
	LOG_CJH(Log, TEXT("🎊 [CalculateFinalWinner] 최종 승자 집계를 시작합니다..."));

	ADDGameStateBase* GameStateBase = GetGameState<ADDGameStateBase>();
	if (!IsValid(GameStateBase)) return;

	TArray<ADDBasePlayerState*> Winners;
	float HighestTrophy = -1.0f;
	float HighestCoin = -1.0f;

	for (APlayerState* PlayerState : GameStateBase->PlayerArray)
	{
		if (ADDBasePlayerState* BasePlayerState = Cast<ADDBasePlayerState>(PlayerState))
		{
			if (!BasePlayerState->bIsParticipant) continue;

			if (BasePlayerState->GetPointSet())
			{
				float CurrentTrophy = BasePlayerState->GetPointSet()->GetTrophy();
				float CurrentCoin = BasePlayerState->GetPointSet()->GetCoin();

				LOG_CJH(Log, TEXT(" - 참여자 [%s] | 트로피: %f | 코인: %f"), *BasePlayerState->PlayerGameData.PlayerDisplayName.ToString(), CurrentTrophy,
				       CurrentCoin);

				// 1순위: 트로피 개수 비교
				if (CurrentTrophy > HighestTrophy)
				{
					HighestTrophy = CurrentTrophy;
					HighestCoin = CurrentCoin;
					Winners.Empty();
					Winners.Add(BasePlayerState);
				}
				// 2순위: 코인 개수 비교 (트로피 동점 시)
				else if (CurrentTrophy == HighestTrophy)
				{
					if (CurrentCoin > HighestCoin)
					{
						HighestCoin = CurrentCoin;
						Winners.Empty();
						Winners.Add(BasePlayerState);
					}
					else if (CurrentCoin == HighestCoin)
					{
						Winners.Add(BasePlayerState);
					}
				}
			}
		}
	}

	if (Winners.Num() > 0)
	{
		FString WinnerNames = TEXT("");
		for (ADDBasePlayerState* Winner : Winners)
		{
			WinnerNames += Winner->PlayerGameData.PlayerDisplayName.ToString() + TEXT(" ");
		}
		LOG_CJH(Log, TEXT("🏆 게임 종료! 승자: %s (트로피: %d, 코인: %d)"), *WinnerNames, (int32)HighestTrophy, (int32)HighestCoin);
	}
	else
	{
		LOG_CJH(Error, TEXT("승자를 찾을 수 없습니다! PointSet 데이터 오류를 확인하세요."));
	}

	SetMatchState(DDGameplayTags::State_BoardGame_End);
}

void ADDGameModeBase::SortPlayersByTurnOrder()
{
	// 1. 최초 게임 진입 시 입장 순서대로 번호를 임시로 부여합니다.
    for (int32 i = 0; i < AlivePlayerControllers.Num(); ++i)
    {
        if (ADDBasePlayerState* BasePlayerState = AlivePlayerControllers[i]->GetPlayerState<ADDBasePlayerState>())
        {
            if (BasePlayerState->PlayerGameData.TurnOrder < 0) 
            {
                BasePlayerState->PlayerGameData.TurnOrder = i;
            }
        }
    }

    // 2. 미니게임에서 갱신되어 넘어온 TurnOrder(0, 1, 2...)를 기준으로 컨트롤러 배열을 재배치(정렬)합니다.
    AlivePlayerControllers.Sort([](const TObjectPtr<APlayerController>& A, const TObjectPtr<APlayerController>& B) {
        ADDBasePlayerState* PlayerStateA = A ? A->GetPlayerState<ADDBasePlayerState>() : nullptr;
        ADDBasePlayerState* PlayerStateB = B ? B->GetPlayerState<ADDBasePlayerState>() : nullptr;
        
        if (PlayerStateA && PlayerStateB)
        {
            return PlayerStateA->PlayerGameData.TurnOrder < PlayerStateB->PlayerGameData.TurnOrder; // 낮은 번호가 먼저 오도록 정렬
        }
    	
        return false;
    });

    // 3. 디버그 로그 출력
    LOG_CJH(Log, TEXT("====================================="));
    LOG_CJH(Log, TEXT("🔄 이번 라운드의 턴 순서가 결정되었습니다!"));
	
    for (int32 i = 0; i < AlivePlayerControllers.Num(); ++i)
    {
        if (ADDBasePlayerState* BasePlayerState = AlivePlayerControllers[i]->GetPlayerState<ADDBasePlayerState>())
        {
            LOG_CJH(Log, TEXT(" [%d] %s (TurnOrder: %d)"), i + 1, *BasePlayerState->PlayerGameData.PlayerDisplayName.ToString(), BasePlayerState->PlayerGameData.TurnOrder);
        }
    }
    LOG_CJH(Log, TEXT("====================================="));
}

UAbilitySystemComponent* ADDGameModeBase::GetAbilitySystemComponentFromPlayer(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return nullptr;

	APlayerState* PlayerState = PlayerController->PlayerState;
	if (!IsValid(PlayerState))
	{
		LOG_CJH(Error, TEXT("[GetAbilitySystemComponentFromPlayer] PlayerState가 존재하지 않습니다!"));
		return nullptr;
	}

	ADDBasePlayerState* BasePlayerState = Cast<ADDBasePlayerState>(PlayerState);
	if (!IsValid(BasePlayerState))
	{
		LOG_CJH(Error, TEXT("[GetAbilitySystemComponentFromPlayer] 타입 캐스팅 실패. 현재 클래스: %s"),
		       *PlayerState->GetClass()->GetName());
		return nullptr;
	}

	if (IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(BasePlayerState))
	{
		return AbilitySystemInterface->GetAbilitySystemComponent();
	}

	return nullptr;
}
