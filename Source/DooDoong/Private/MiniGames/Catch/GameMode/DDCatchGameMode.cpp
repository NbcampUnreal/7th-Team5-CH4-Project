#include "MiniGames/Catch/GameMode/DDCatchGameMode.h"

#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "Common/DDLogManager.h"
#include "MiniGames/Catch/Actors/DiamondSpawner.h"
#include "MiniGames/Catch/GameState/DDCatchGameState.h"

ADDCatchGameMode::ADDCatchGameMode()
{
	GameStateClass = ADDCatchGameState::StaticClass();
}

void ADDCatchGameMode::StartMiniGame()
{
	Super::StartMiniGame();

	LOG_CYS(Warning,TEXT("다이아 캐치캐치 시작"));
	AActor* FoundCamera = nullptr;

	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(TEXT("GameCamera")))
		{
			FoundCamera = *It;
			break;
		}
	}

	if (!FoundCamera) return;

	// 현재 실행 타이밍이 너무 빠를 수 있음. 나중에 플레이어 레디 추가 후 다시 로직 수정할 예정.
	ADDCatchGameState* GS = GetGameState<ADDCatchGameState>();
	if (GS)
	{
		GS->Multicast_SetGameCamera(FoundCamera);
	}
	
	// 스포너 시작
	for (TActorIterator<ADiamondSpawner> It(GetWorld()); It; ++It)
	{
		ADiamondSpawner* Spawner = *It;
		if (Spawner)
		{
			LOG_CYS(Warning,TEXT("다이아를 스폰해라."))
			Spawner->StartSpawn();
		}
	}
}

void ADDCatchGameMode::FinishMiniGame()
{
	Super::FinishMiniGame();
	
	for (TActorIterator<ADiamondSpawner> It(GetWorld()); It; ++It)
	{
		ADiamondSpawner* Spawner = *It;
		if (Spawner)
		{
			Spawner->StopSpawn();
		}
	}
}
