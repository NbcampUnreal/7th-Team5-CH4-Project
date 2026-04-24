#include "MiniGames/Platformer/GameState/DDPlatformerGameState.h"

#include "Base/Player/DDBasePlayerController.h"

ADDPlatformerGameState::ADDPlatformerGameState()
	: WaitingTime(10.f)
{
	
}

void ADDPlatformerGameState::BeginPlay()
{
	Super::BeginPlay();
	//@TODO_Minjae : 게임입장하면 플레이어 컨트롤러 정보 받아와서 대기시간동안 조작못하게 멈추기
	
}

void ADDPlatformerGameState::WaitingGame(APlayerController* LoginPlayer)
{
	if(LoginPlayer == nullptr)
	{
		return;
	}
	
	ADDBasePlayerController* DDPlatformerPlayerController = Cast<ADDBasePlayerController>(LoginPlayer);
	if (DDPlatformerPlayerController != nullptr)
	{
		FInputModeUIOnly UIMode;
		DDPlatformerPlayerController->SetInputMode(UIMode);
	}
}
