// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGames/Platformer/GameMode/DDPlatformerGameModeBase.h"
#include "Base/Player/DDBasePlayerController.h"

void ADDPlatformerGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	ADDBasePlayerController* DDPlayerController = Cast<ADDBasePlayerController>(NewPlayer);
	if (IsValid(DDPlayerController) == true)
	{
		AllPlayerControllers.Add(DDPlayerController);
	}
}

void ADDPlatformerGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	
}
