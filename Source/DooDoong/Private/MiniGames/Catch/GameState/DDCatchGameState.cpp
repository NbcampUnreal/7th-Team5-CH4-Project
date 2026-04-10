#include "MiniGames/Catch/GameState/DDCatchGameState.h"
#include "Common/DDLogManager.h"
#include "Kismet/GameplayStatics.h"

ADDCatchGameState::ADDCatchGameState()
{
}

void ADDCatchGameState::Multicast_SetGameCamera_Implementation(AActor* CameraActor)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC || !CameraActor) return;

	PC->SetViewTargetWithBlend(CameraActor, 0.5f);
	LOG_CYS(Warning,TEXT("[GS] 게임 카메라 전환"));
}

