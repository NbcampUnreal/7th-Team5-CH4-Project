#include "MiniGames/Platformer/GameMode/DDPlatformerGameMode.h"
#include "MiniGames/Platformer/GameState/DDPlatformerGameState.h"
#include "Base/Player/DDBasePlayerController.h"
#include "Base/Player/DDBasePlayerState.h"
#include "Base/Character/DDBaseCharacter.h"
#include "System/DDGameplayTags.h"
#include "Common/DDLogManager.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

void ADDPlatformerGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	/* 캐릭터 고유 ID 생길경우 TMap 으로 리펙토링 예정 */
	ADDBasePlayerController* PlayerController = Cast<ADDBasePlayerController>(NewPlayer);
	if (IsValid(PlayerController) == true)
	{
		AllPlayerControllers.Add(PlayerController);
		ADDBaseCharacter* PlayerCharacter = Cast<ADDBaseCharacter>(PlayerController->GetPawn());
		if (IsValid(PlayerCharacter) == true)
		{
			AllPlayerCharacters.Add(PlayerCharacter);
		}
		//TODO_@Minjae : Setter 함수 완성되면 DA랑 IMC 넘겨주기
	}
}

void ADDPlatformerGameMode::HandleStartingNewPlayer_Implementation(APlayerController* PlatformerEnteredPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(PlatformerEnteredPlayer);
	
	ADDBasePlayerController* BasePlayerController = Cast<ADDBasePlayerController>(PlatformerEnteredPlayer);
	if (IsValid(BasePlayerController) == true)
	{
		ADDBasePlayerState* BasePlayerState = BasePlayerController->GetPlayerState<ADDBasePlayerState>();
		if (IsValid(BasePlayerState) == true)
		{
			//플레이어 게임설명UI창 열어주기
			//PlatformerEnteredPlayer->OpenReadyUI();
			FPlatformerPlayerData PlayerData;
			PlayerData.PlayerController = BasePlayerController;
			PlayerData.PlayerState = BasePlayerState;
			PlayerData.PlayerMaxDistance = 0;
			PlayerData.PlayerRank = -1;
			PlayerData.PlayerSlotIndex = BasePlayerState->PlayerGameData.SlotIndex;
			PlayerData.PlayerDisplayName = BasePlayerState->PlayerGameData.PlayerNickName;
			PlayerData.PlayerColor = BasePlayerState->PlayerGameData.PlayerColor;
			LOG_PMJ(Warning, TEXT("PlayerSlotIndex : %d"), PlayerData.PlayerSlotIndex);
			PlayerDatas.Add(PlayerData.PlayerSlotIndex, PlayerData);
		}
	}
}

void ADDPlatformerGameMode::BeginPlay()
{
	Super::BeginPlay();
	LOG_PMJ(Warning, TEXT("BeginPlayServer"));
	/* 게임모드 일시정지 */
	//UGameplayStatics::SetGamePaused(GetWorld(), true);
	
	/* 시작지점 초기화 각 캐릭터가 시작지점을 기준으로 얼마나 멀리갔는지 최고기록 체크를위함 */
	StartLocation = FVector(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < AllPlayerCharacters.Num(); i++)
	{
		PlayerMaxDistances[i] = 0.0f;
	}
	
	AGameStateBase* GameStateBase = Cast<AGameStateBase>(GetWorld()->GetGameState());
	if (IsValid(GameStateBase) == true)
	{
		PlatformerGameStateBase = Cast<ADDPlatformerGameState>(GameStateBase);
	}
	
	WaitingTimerStart();
}

void ADDPlatformerGameMode::GameStart()
{
	UE_LOG(LogPMJ, Log, TEXT("GameStart"));
	/* 게임모드일시정지 해제*/
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	PlatformerGameStateBase->SetMiniGameState(DDGameplayTags::State_MiniGame_Playing);
	GetWorldTimerManager().ClearTimer(FinishedWaitingTimerHandle);
	GetWorldTimerManager().SetTimer(
		PlatformerPlayTimerHandle,
		this,
		&ADDPlatformerGameMode::GameEnd,
		PlatformerPlayTime,
		false
		);
	
	GetWorldTimerManager().SetTimer(
		DistanceTimerHandle,
		this,
		&ADDPlatformerGameMode::PlayGameTimer,
		1.f,
		true
		);
}

void ADDPlatformerGameMode::GameEnd()
{
	UE_LOG(LogPMJ, Log, TEXT("GameEnd"));
	GetWorldTimerManager().ClearTimer(DistanceTimerHandle);
	//TODO_@Minjae : 점수 계산해서 넘겨주는 로직 구현
	
	PlayerRanking();
	
}

void ADDPlatformerGameMode::PlayGameTimer()
{
	UE_LOG(LogPMJ, Log, TEXT("PlayGameTimer"));
	/* 게임플레이 타이머함수가 호출될때마다 각 플레이어의 최고 거리를 기록 */
	for (int i = 0; i < AllPlayerCharacters.Num(); i++)
	{
		float CurrentPlayerDistance = FVector::Dist(StartLocation, AllPlayerCharacters[i]->GetActorLocation());
		if (PlayerMaxDistances[i] < CurrentPlayerDistance)
		{
			PlayerMaxDistances[i] = CurrentPlayerDistance;
		}
	}
}

void ADDPlatformerGameMode::CheckGoalPlayerCharacter(AActor* GoalActor)
{
	/* 일단은 간략하게 순위정하는거 구현해놓을건데 플레이어별로 구분할 수 있는 태그만들어서 나중에 리펙토링할게요 */
	APawn* PlayerPawn = Cast<APawn>(GoalActor);
	if (IsValid(PlayerPawn) == true)
	{
		ADDBaseCharacter* PlayerCharacter = Cast<ADDBaseCharacter>(PlayerPawn);
		if (IsValid(PlayerCharacter) == true)
		{
			ADDBasePlayerController* BasePlayerController = Cast<ADDBasePlayerController>(PlayerCharacter->GetController());
			if (IsValid(BasePlayerController) == true)
			{
				 /* TODO_@Minjae : 플레이어 구조체 접근해서 slotindex 로 골인한 플레이어 찾아서 순위 매기기 */
				ADDBasePlayerState* BasePlayerState = BasePlayerController->GetPlayerState<ADDBasePlayerState>();
				if (IsValid(BasePlayerState) == true)
				{
					for (TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerDatas)
					{
						if (EnteredPlayer.Value.PlayerSlotIndex == BasePlayerState->PlayerGameData.SlotIndex)
						{
							if (EnteredPlayer.Value.bIsGoalIn == false)
							{
								EnteredPlayer.Value.PlayerRank = Rank;
								LOG_PMJ(Warning, TEXT("PlayerRank : %d"), EnteredPlayer.Value.PlayerRank);
								Rank++;
								EnteredPlayer.Value.bIsGoalIn = true;
								PlayerRankingArrays.Add(EnteredPlayer.Value.PlayerController);
							}
							else
							{
								LOG_PMJ(Warning, TEXT("해당플레이어는 이미 %d등 입니다"), EnteredPlayer.Value.PlayerRank);
							}
						}
					}
				}
			}
		}
	}
}

void ADDPlatformerGameMode::PlayerRanking()
{
	UE_LOG(LogPMJ, Log, TEXT("PlayerRanking"));
	
	
	if (PlayerRankingArrays.Num() != AllPlayerControllers.Num())
	{
		int32 Index = AllPlayerCharacters.Num();
		for (int i = 0; i < Index; i++)
		{
			float MaxValue = PlayerMaxDistances[i];
			for (int j = 0; j < PlayerMaxDistances.Num(); j++)
			{
				if (MaxValue < PlayerMaxDistances[j])
				{
					MaxValue = PlayerMaxDistances[j];
				}
			}
			
			for (int j = 0; j < PlayerMaxDistances.Num(); j++)
			{
				if (MaxValue == PlayerMaxDistances[j])
				{
					PlayerRankingArrays.Add(AllPlayerCharacters[j]);
					AllPlayerCharacters.RemoveAt(j);
					PlayerMaxDistances.RemoveAt(j);
				}
			}
		}
	}
	
	//TODO_@Minjae : AddScore 로직 필요
}

void ADDPlatformerGameMode::WaitingTimerStart()
{
	PlatformerGameStateBase->SetMiniGameState(DDGameplayTags::State_MiniGame_Preparing);
	
	UE_LOG(LogPMJ, Log, TEXT("WaitingTimerStart"));
	
	GetWorldTimerManager().SetTimer(
	FinishedWaitingTimerHandle,
	this,
	&ADDPlatformerGameMode::GameStart,
	10.f,
	false,
	10.f);
}

void ADDPlatformerGameMode::CheckReadyPlayers()
{
	//TODO_@Minjae : 플레이어들이 준비되었는지 확인하는 로직 구현 
	//플레이어쪽에서 대기화면이 나타났을때 준비완료 버튼을 누를때마다 이벤트 함수 호출
	//그때마다 준비상태를 확인하고 4명이 준비완료되었을경우 게임 시작
	
	/*if (AllPlayerControllers.IsEmpty() == true)
	{
		return;
	}
	
	for (const TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerDatas)
	{
		
	}
	
	if (AllPlayerControllers[0]->bIsReady == true && 
		AllPlayerControllers[1]->bIsReady == true &&
		AllPlayerControllers[2]->bIsReady == true &&
		AllPlayerControllers[3]->bIsReady == true )
	{
		WaitingTimerStart();
	}*/
	
}

void ADDPlatformerGameMode::GetPlayerSlotIndex()
{
	/*for (TActorIterator<ADDBasePlayerController> It(GetWorld()); It; ++It)
	{
		UE_LOG(LogPMJ, Log, TEXT("ActorIterator"));
		ADDBasePlayerController* PlayerController = Cast<ADDBasePlayerController>(*It);
		if (IsValid(PlayerController) == true)
		{
			/* PlayerState구조체 접근해서 SlotIndex 가져오기 #1#
			int32 SlotIndex = PlayerInfo.SlotIndex;
			FName PlayerName(FString::Printf(TEXT("Player%d"), SlotIndex));
			PlayerDatas.Add(FName(PlayerName), PlayerController);
			UE_LOG(LogPMJ, Log, TEXT("Player slotindex: %d"), SlotIndex);
		}
	}*/
	
}

