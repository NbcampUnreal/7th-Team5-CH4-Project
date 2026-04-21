#include "MiniGames/Platformer/GameMode/DDPlatformerGameMode.h"
#include "MiniGames/Platformer/GameState/DDPlatformerGameState.h"
#include "Base/Player/DDBasePlayerController.h"
#include "Base/Player/DDBasePlayerState.h"
#include "Base/Character/DDBaseCharacter.h"
#include "System/DDGameplayTags.h"
#include "Common/DDLogManager.h"

void ADDPlatformerGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
	
	LOG_PMJ(Warning, TEXT("HandleSeamlessTravel"));
	ADDBasePlayerController* BasePlayerController = Cast<ADDBasePlayerController>(C);
	if (IsValid(BasePlayerController) == true)
	{
		ADDBasePlayerState* BasePlayerState = BasePlayerController->GetPlayerState<ADDBasePlayerState>();
		if (IsValid(BasePlayerState) == true)
		{
			FPlatformerPlayerData PlayerData;
			PlayerData.PlayerController = BasePlayerController;
			PlayerData.PlayerState = BasePlayerState;
			PlayerData.PlayerMaxDistance = 0;
			PlayerData.PlayerRank = -1;
			PlayerData.SavePointLocation = FVector(0.f,0.f,0.f);
			PlayerData.PlayerSlotIndex = BasePlayerState->PlayerGameData.SlotIndex;
			PlayerData.PlayerDisplayName = BasePlayerState->PlayerGameData.PlayerDisplayName.ToString();
			PlayerData.PlayerColor = BasePlayerState->PlayerGameData.PlayerColor;
			PlayerData.bIsGoalIn = false;
			LOG_PMJ(Warning, TEXT("PlayerSlotIndex : %d"), PlayerData.PlayerSlotIndex);
			PlayerDatas.Add(PlayerData.PlayerSlotIndex, PlayerData);
		}
	}
}

void ADDPlatformerGameMode::BeginPlay()
{
	Super::BeginPlay();
	LOG_PMJ(Warning, TEXT("BeginPlayServer"));
	/* 시작지점 초기화 각 캐릭터가 시작지점을 기준으로 얼마나 멀리갔는지 최고기록 체크를위함 */
	StartLocation = FVector(0.0f, 0.0f, 0.0f);
	
	AGameStateBase* GameStateBase = Cast<AGameStateBase>(GetWorld()->GetGameState());
	if (IsValid(GameStateBase) == true)
	{
		PlatformerGameStateBase = Cast<ADDPlatformerGameState>(GameStateBase);
	}
	MaxPlayer = PlayerDatas.Num();
}

void ADDPlatformerGameMode::StartMiniGame()
{
	Super::StartMiniGame();
	GameStart();
}

void ADDPlatformerGameMode::GameStart()
{
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

void ADDPlatformerGameMode::PlayGameTimer()
{
	UE_LOG(LogPMJ, Log, TEXT("PlayGameTimer"));
	/* 게임플레이 타이머함수가 호출될때마다 각 플레이어의 최고 거리를 기록 */
	for (TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerDatas)
	{
		FVector PlayerLocation = EnteredPlayer.Value.PlayerController->GetCharacter()->GetActorLocation();
		float CurrentPlayerDistance = FVector::Dist(StartLocation, PlayerLocation);
		if (EnteredPlayer.Value.PlayerMaxDistance < CurrentPlayerDistance)
		{
			EnteredPlayer.Value.PlayerMaxDistance = CurrentPlayerDistance;
		}
	}
}

void ADDPlatformerGameMode::GameEnd()
{
	UE_LOG(LogPMJ, Log, TEXT("GameEnd"));
	GetWorldTimerManager().ClearTimer(DistanceTimerHandle);
	//TODO_@Minjae : 점수 계산해서 넘겨주는 로직 구현
	
	for (const TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerDatas)
	{
		if (EnteredPlayer.Value.bIsGoalIn == false)
		{
			PlayerNoGoalArrays.Add(EnteredPlayer);
		}
	}
	
	PlayerRanking();
	
}

void ADDPlatformerGameMode::CheckGoalPlayerCharacter(AActor* GoalActor)
{
	/* 일단은 간략하게 순위정하는거 구현해놓을건데 플레이어별로 구분할 수 있는 태그만들어서 나중에 리펙토링할게요 */
	APawn* PlayerPawn = Cast<APawn>(GoalActor);
	if (IsValid(PlayerPawn) == true)
	{
		/* 플레이어 스테이트 캐스팅 */
		ADDBasePlayerState* BasePlayerState = PlayerPawn->GetPlayerState<ADDBasePlayerState>();
		if (IsValid(BasePlayerState) == true)
		{
			/* 입장한 플레이어 순회 */
			for (TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerDatas)
			{
				/* 골인한 플레이어 를 찾았다면 */
				if (EnteredPlayer.Value.PlayerSlotIndex == BasePlayerState->PlayerGameData.SlotIndex)
				{
					/* 골인 안한 상태라면 */
					if (EnteredPlayer.Value.bIsGoalIn == false)
					{
						/* 순위 지정하고 다음순위 수정 */
						EnteredPlayer.Value.PlayerRank = Rank;
						LOG_PMJ(Warning, TEXT("PlayerRank : %d"), EnteredPlayer.Value.PlayerRank);
						Rank++;
						EnteredPlayer.Value.bIsGoalIn = true;
						/* 랭킹배열에 넣고 */
						PlayerRankingArrays.Add(EnteredPlayer);
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

void ADDPlatformerGameMode::PlayerRanking()
{
	UE_LOG(LogPMJ, Log, TEXT("PlayerRanking"));
	/* 랭킹배열 돌았는데 인원이 맞지 않고 */
	if (PlayerRankingArrays.Num() != MaxPlayer)
	{
		/* 골인을 하지 못한 플레이어가 존재한다면 */
		if (PlayerNoGoalArrays.IsEmpty() == false)
		{
			/* 골인 못한 플레이어들 최대 이동거리별로 정렬 */
			PlayerNoGoalArrays.ValueSort([](
			const FPlatformerPlayerData& A,const FPlatformerPlayerData& B)
			{
				return A.PlayerMaxDistance > B.PlayerMaxDistance;
			});
			
			/* 정렬된 순서대로 높은 등수 부여 */
			for (TPair<int32, FPlatformerPlayerData>& EnteredPlayerNoGoal : PlayerNoGoalArrays)
			{
				EnteredPlayerNoGoal.Value.PlayerRank = Rank;
				Rank++;
				PlayerRankingArrays.Add(EnteredPlayerNoGoal);
			}
		}
	}
	
	for (const TPair<int32, FPlatformerPlayerData>& EnteredPlayerRanking : PlayerRankingArrays)
	{
		LOG_PMJ(Warning, TEXT("PlayerID : %d PlayerRank : %d"), EnteredPlayerRanking.Value.PlayerSlotIndex, EnteredPlayerRanking.Value.PlayerRank);
	}
	
	//TODO_@Minjae : AddScore 로직 필요
	
	for (const TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerDatas)
	{
		switch (EnteredPlayer.Value.PlayerRank)
		{
		case 1 : AddScore(EnteredPlayer.Value.PlayerState.Get(), 100);
			LOG_PMJ(Error, TEXT("=== GAMEMODE : %d Player, AddScore : 100, Rank : %d ==="), EnteredPlayer.Value.PlayerSlotIndex, EnteredPlayer.Value.PlayerRank);
			break;
		case 2 : AddScore(EnteredPlayer.Value.PlayerState.Get(), 80);
			LOG_PMJ(Error, TEXT("=== GAMEMODE : %d Player, AddScore : 80, Rank : %d ==="), EnteredPlayer.Value.PlayerSlotIndex, EnteredPlayer.Value.PlayerRank);
			break;
		case 3 : AddScore(EnteredPlayer.Value.PlayerState.Get(), 60);
			LOG_PMJ(Error, TEXT("=== GAMEMODE : %d Player, AddScore : 60, Rank : %d ==="), EnteredPlayer.Value.PlayerSlotIndex, EnteredPlayer.Value.PlayerRank);
			break;
		case 4 : AddScore(EnteredPlayer.Value.PlayerState.Get(), 40);
			LOG_PMJ(Error, TEXT("=== GAMEMODE : %d Player, AddScore : 40, Rank : %d ==="), EnteredPlayer.Value.PlayerSlotIndex, EnteredPlayer.Value.PlayerRank);
			break;
		default: LOG_PMJ(Error, TEXT("=== GAMEMODE : 플레이어의 랭킹 없음 ==="));
			break;
		}
	}
	
	GetWorldTimerManager().SetTimer(
		FinishGameTimerHandle,
		this,
		&ADDPlatformerGameMode::CallFinishGameTimer,
		15.f,
		false
		);
}

void ADDPlatformerGameMode::CallFinishGameTimer()
{
	FinishMiniGame();
}

/*void ADDPlatformerGameMode::WaitingTimerStart()
{
	PlatformerGameStateBase->SetMiniGameState(DDGameplayTags::State_MiniGame_Preparing);
	
	for (const TPair<int32 ,FPlatformerPlayerData>& EnteredPlayer : PlayerDatas)
	{
		EnteredPlayer.Value.PlayerState->PlayerGameData.bPlayerIsDead = false;
	}
	
	UE_LOG(LogPMJ, Log, TEXT("WaitingTimerStart"));
	
	GetWorldTimerManager().SetTimer(
	FinishedWaitingTimerHandle,
	this,
	&ADDPlatformerGameMode::GameStart,
	10.f,
	false);
}*/

/*void ADDPlatformerGameMode::CheckReadyPlayers()
{
	//TODO_@Minjae : 플레이어들이 준비되었는지 확인하는 로직 구현 
	//플레이어쪽에서 대기화면이 나타났을때 준비완료 버튼을 누를때마다 이벤트 함수 호출
	//그때마다 준비상태를 확인하고 4명이 준비완료되었을경우 게임 시작
	
	if (PlayerDatas.IsEmpty() == true)
	{
		return;
	}
	bool bIsAllReady = true;
	
	for (const TPair<int32 ,FPlatformerPlayerData>& EnteredPlayer : PlayerDatas)
	{
		if (EnteredPlayer.Value.PlayerState->PlayerGameData.bPlayerIsDead == false)
		{
			bIsAllReady = false;
		}
	}
	
	if (bIsAllReady)
	{
		WaitingTimerStart();
	}
}*/

