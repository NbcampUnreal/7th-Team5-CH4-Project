#include "MiniGames/Batter/GameMode/DDBatterGameMode.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "Common/DDLogManager.h"
#include "MiniGames/Catch/GameState/DDCatchGameState.h"

ADDBatterGameMode::ADDBatterGameMode()
{
	GameStateClass = ADDBatterGameMode::StaticClass();
}

void ADDBatterGameMode::StartMiniGame()
{
	Super::StartMiniGame();
	
	LOG_CYS(Warning,TEXT("연타 게임 시작"));
	AActor* FoundCamera = nullptr;

	// 카메라 찾기
	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(TEXT("GameCamera")))
		{
			FoundCamera = *It;
			break;
		}
	}

	if (!FoundCamera) return;
	// 카메라 전환 지시
	ADDCatchGameState* GS = GetGameState<ADDCatchGameState>();
	if (GS)
	{
		GS->Multicast_SetGameCamera(FoundCamera);
	}
}

void ADDBatterGameMode::FinishMiniGame()
{
	Super::FinishMiniGame();
}
