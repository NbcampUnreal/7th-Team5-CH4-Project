#include "MiniGames/Platformer/GameMode/DDPlatformerGameMode.h"
#include "MiniGames/Platformer/GameState/DDPlatformerGameState.h"
#include "Base/Player/DDBasePlayerController.h"
#include "Base/Player/DDBasePlayerState.h"
#include "Base/Character/DDBaseCharacter.h"
#include "System/DDGameplayTags.h"
#include "Common/DDLogManager.h"

void ADDPlatformerGameMode::HandleStartingNewPlayer_Implementation(APlayerController* PlatformerEnteredPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(PlatformerEnteredPlayer);
	
	ADDBasePlayerController* BasePlayerController = Cast<ADDBasePlayerController>(PlatformerEnteredPlayer);
	if (IsValid(BasePlayerController) == true)
	{
		ADDBasePlayerState* BasePlayerState = BasePlayerController->GetPlayerState<ADDBasePlayerState>();
		if (IsValid(BasePlayerState) == true)
		{
			BasePlayerState->PlayerGameData.SlotIndex = PlayerIndex;
			PlayerIndex++;
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
	//UGameplayStatics::SetGamePaused(GetWorld(), false);
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
	
	for (const TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerDatas)
	{
		if (EnteredPlayer.Value.bIsGoalIn == false)
		{
			PlayerNoGoalArrays.Add(EnteredPlayer);
		}
	}
	
	PlayerRanking();
	
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

void ADDPlatformerGameMode::CheckGoalPlayerCharacter(AActor* GoalActor)
{
	/* 일단은 간략하게 순위정하는거 구현해놓을건데 플레이어별로 구분할 수 있는 태그만들어서 나중에 리펙토링할게요 */
	APawn* PlayerPawn = Cast<APawn>(GoalActor);
	if (IsValid(PlayerPawn) == true)
	{
		/* 캐릭터 캐스팅 */
		ADDBaseCharacter* PlayerCharacter = Cast<ADDBaseCharacter>(PlayerPawn);
		if (IsValid(PlayerCharacter) == true)
		{
			/* 컨트롤러 캐스팅 */
			ADDBasePlayerController* BasePlayerController = Cast<ADDBasePlayerController>(PlayerCharacter->GetController());
			if (IsValid(BasePlayerController) == true)
			{
				/* 플레이어 스테이트 캐스팅 */
				ADDBasePlayerState* BasePlayerState = BasePlayerController->GetPlayerState<ADDBasePlayerState>();
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
	}
}

void ADDPlatformerGameMode::PlayerRanking()
{
	UE_LOG(LogPMJ, Log, TEXT("PlayerRanking"));
	/* 랭킹배열 돌았는데 인원이 맞지 않고 */
	if (PlayerRankingArrays.Num() != MaxPlayer)
	{
		/* 점수산정해야하는 플레이어가 비어있지않다면 */
		if (PlayerNoGoalArrays.IsEmpty() == false)
		{
			/* 골인 못한 플레이어들 최대 이동거리별로 정렬 */
			PlayerNoGoalArrays.ValueSort([](
			const FPlatformerPlayerData& A,const FPlatformerPlayerData& B)
			{
				return A.PlayerMaxDistance > B.PlayerMaxDistance;
			});
			
			/* 정렬된 순서대로 높은 등수 부여 */
			for (TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerNoGoalArrays)
			{
				EnteredPlayer.Value.PlayerRank = Rank;
				Rank++;
				PlayerRankingArrays.Add(EnteredPlayer);
			}
		}
	}
	for (const TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerRankingArrays)
	{
		LOG_PMJ(Warning, TEXT("PlayerID : %d PlayerRank : %d"), EnteredPlayer.Value.PlayerSlotIndex, EnteredPlayer.Value.PlayerRank);
	}
	
	//TODO_@Minjae : AddScore 로직 필요
}

void ADDPlatformerGameMode::WaitingTimerStart()
{
	PlatformerGameStateBase->SetMiniGameState(DDGameplayTags::State_MiniGame_Preparing);
	
	for (const TPair<int32 ,FPlatformerPlayerData>& EnteredPlayer : PlayerDatas)
	{
		EnteredPlayer.Value.PlayerState->PlayerGameData.bIsReady = false;
	}
	
	UE_LOG(LogPMJ, Log, TEXT("WaitingTimerStart"));
	
	GetWorldTimerManager().SetTimer(
	FinishedWaitingTimerHandle,
	this,
	&ADDPlatformerGameMode::GameStart,
	10.f,
	false);
}

void ADDPlatformerGameMode::CheckReadyPlayers()
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
		if (EnteredPlayer.Value.PlayerState->PlayerGameData.bIsReady == false)
		{
			bIsAllReady = false;
		}
	}
	
	if (bIsAllReady)
	{
		WaitingTimerStart();
	}
}

