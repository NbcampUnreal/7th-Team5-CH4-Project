#include "MiniGames/Platformer/GameMode/DDPlatformerGameMode.h"
#include "MiniGames/Platformer/Character/DDPlatformerCharacter.h"
#include "Base/Player/DDBasePlayerState.h"

#include "Common/DDLogManager.h"
void ADDPlatformerGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
	
	LOG_PMJ(Warning, TEXT("HandleSeamlessTravel"));
}

void ADDPlatformerGameMode::BeginPlay()
{
	Super::BeginPlay();
	LOG_PMJ(Warning, TEXT("BeginPlayServer"));
	StartLocation = FVector2D(0.0f, 0.0f);
}

void ADDPlatformerGameMode::SetEnteredPlayer()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC == nullptr)
		{
			continue;
		}
		
		APawn* Pawn = PC->GetPawn();
		if (IsValid(Pawn))
		{
			ADDPlatformerCharacter* DDCharacter = Cast<ADDPlatformerCharacter>(Pawn);
			if (!IsValid(DDCharacter)) return;
			EnteredPlayers.Add(DDCharacter);
		}
	}
}

void ADDPlatformerGameMode::StartMiniGame()
{
	Super::StartMiniGame();
	SetEnteredPlayer();
	
	LOG_PMJ(Warning, TEXT("EnteredPlayer : %d"), EnteredPlayers.Num());
	
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
	/* 게임플레이 타이머함수가 호출될때마다 각 플레이어의 최고 거리를 기록 */
	
	for (ADDPlatformerCharacter* DDPlayerCharacter : EnteredPlayers)
	{
		FVector PlayerLocation = DDPlayerCharacter->GetActorLocation();
		FVector2D PlayerLocation2D = FVector2D(PlayerLocation.X, PlayerLocation.Y);
		float PlayerCurrentDistance = FVector2D::Distance(StartLocation, PlayerLocation2D);
		if (DDPlayerCharacter->PlayerMaxDistance < PlayerCurrentDistance)
		{
			DDPlayerCharacter->PlayerMaxDistance = PlayerCurrentDistance;
			LOG_PMJ(Warning, TEXT("Player : %d, MaxDistance : %f"), 
				DDPlayerCharacter->GetPlayerState()->GetPlayerId(), DDPlayerCharacter->PlayerMaxDistance);
		}
	}
}

void ADDPlatformerGameMode::ReturnToSavePoint(AActor* OverlapActor)
{
	if (!OverlapActor) return;
	
	ADDPlatformerCharacter* DDOverlapPlayer = Cast<ADDPlatformerCharacter>(OverlapActor);
	if (!IsValid(DDOverlapPlayer)) return;
	
	DDOverlapPlayer->Server_ReturnToSavePoint();
	
}

void ADDPlatformerGameMode::UpdatePlayerSavePoint(AActor* OverlapActor, const FVector UpdateSavePoint)
{
	if (!OverlapActor) return;
	
	ADDPlatformerCharacter* DDOverlapPlayer = Cast<ADDPlatformerCharacter>(OverlapActor);
	if (!IsValid(DDOverlapPlayer)) return;
	
	DDOverlapPlayer->Server_SetSavePointLocation(UpdateSavePoint);
}

void ADDPlatformerGameMode::CheckGoalPlayerCharacter(AActor* OverlapActor)
{
	/* 일단은 간략하게 순위정하는거 구현해놓을건데 플레이어별로 구분할 수 있는 태그만들어서 나중에 리펙토링할게요 */
	if (!OverlapActor) return;
	
	ADDPlatformerCharacter* GoalPlayer = Cast<ADDPlatformerCharacter>(OverlapActor);
	if (!IsValid(GoalPlayer)) return;
	
	GoalPlayer->PlayerRank = Rank;
	Rank++;
	GoalPlayer->bIsGoalIn = true;
}

void ADDPlatformerGameMode::GameEnd()
{
	UE_LOG(LogPMJ, Log, TEXT("GameEnd"));
	GetWorldTimerManager().ClearTimer(DistanceTimerHandle);
	
	for (ADDPlatformerCharacter* DDPlayerCharacter : EnteredPlayers)
	{
		if (DDPlayerCharacter->bIsGoalIn == false)
		{
			//남은거리 비교를 위해 NoGoalPlayer.Add
			NoGoalPlayers.Add(DDPlayerCharacter);
		}
	}
	
	PlayerRanking();
	
}


void ADDPlatformerGameMode::PlayerRanking()
{
	LOG_PMJ(Warning, TEXT("PlayerRanking"));
	
	for (ADDPlatformerCharacter* NoGoalPlayer : NoGoalPlayers)
	{
		if (NoGoalPlayer->bIsGoalIn == false)
		{
			NoGoalPlayers.Sort([](const ADDPlatformerCharacter& A, const ADDPlatformerCharacter& B)
			{
				return A.PlayerMaxDistance > B.PlayerMaxDistance;
			});
		}
	}
	
	for (ADDPlatformerCharacter* NoGoalPlayer : NoGoalPlayers)
	{
		if (NoGoalPlayer->PlayerRank < 1)
		{
			NoGoalPlayer->PlayerRank = Rank;
			Rank++;
		}
	}
	
	for (ADDPlatformerCharacter* DDPlayer : EnteredPlayers)
	{
		LOG_PMJ(Warning, TEXT("PlayerID : %d PlayerRank : %d"), DDPlayer->GetPlayerState()->GetPlayerId(), DDPlayer->PlayerRank);
	}
	
	for (ADDPlatformerCharacter* DDPlayer : EnteredPlayers)
	{
		switch (DDPlayer->PlayerRank)
		{
		case 1 : AddScore(DDPlayer->GetPlayerState(), 100);
			LOG_PMJ(Error, TEXT("=== GAMEMODE : %d Player, AddScore : 100, Rank : %d ==="), DDPlayer->GetPlayerState()->GetPlayerId(), DDPlayer->PlayerRank);
			break;
		case 2 : AddScore(DDPlayer->GetPlayerState(), 80);
			LOG_PMJ(Error, TEXT("=== GAMEMODE : %d Player, AddScore : 80, Rank : %d ==="), DDPlayer->GetPlayerState()->GetPlayerId(), DDPlayer->PlayerRank);
			break;
		case 3 : AddScore(DDPlayer->GetPlayerState(), 60);
			LOG_PMJ(Error, TEXT("=== GAMEMODE : %d Player, AddScore : 60, Rank : %d ==="), DDPlayer->GetPlayerState()->GetPlayerId(), DDPlayer->PlayerRank);
			break;
		case 4 : AddScore(DDPlayer->GetPlayerState(), 40);
			LOG_PMJ(Error, TEXT("=== GAMEMODE : %d Player, AddScore : 40, Rank : %d ==="), DDPlayer->GetPlayerState()->GetPlayerId(), DDPlayer->PlayerRank);
			break;
		default: LOG_PMJ(Warning, TEXT("=== GAMEMODE : 플레이어의 랭킹 없음 ==="));
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



