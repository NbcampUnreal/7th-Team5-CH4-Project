// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGames/Platformer/GameMode/DDPlatformerGameModeBase.h"
#include "MiniGames/Platformer/GameState/DDPlatformerGameStateBase.h"
#include "Base/Player/DDBasePlayerController.h"
#include "Common/LogManager.h"
#include "System/DDGameplayTags.h"
#include "Common/LogManager.h"

void ADDPlatformerGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	ADDBasePlayerController* DDPlayerController = Cast<ADDBasePlayerController>(NewPlayer);
	if (IsValid(DDPlayerController) == true)
	{
		AllPlayerControllers.Add(DDPlayerController);
		//TODO_@Minjae : Setter 함수 완성되면 DA랑 IMC 넘겨주기
	}
}

void ADDPlatformerGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	AGameStateBase* GameStateBase = Cast<AGameStateBase>(GetWorld()->GetGameState());
	if (IsValid(GameStateBase) == true)
	{
		PlatformerGameStateBase = Cast<ADDPlatformerGameStateBase>(GameStateBase);
	}
	
	WaitingTimerStart();
}

void ADDPlatformerGameModeBase::FinishedWaitingTimer()
{
	PlatformerGameStateBase->SetMiniGameState(DDGameplayTags::State_MiniGame_Playing);
	UE_LOG(LogPMJ, Log, TEXT("FinishedWaitingTimer"));
}

void ADDPlatformerGameModeBase::WaitingTimerStart()
{
	PlatformerGameStateBase->SetMiniGameState(DDGameplayTags::State_MiniGame_Preparing);
	UE_LOG(LogPMJ, Log, TEXT("WaitingTimerStart"));
	
	GetWorld()->GetTimerManager().SetTimer(
	FinishedWaitingTimerHandle,
	this,
	&ADDPlatformerGameModeBase::FinishedWaitingTimer,
	10.f,
	false
	);
}

void ADDPlatformerGameModeBase::CheckReadyPlayers()
{
	//TODO_@Minjae : 플레이어들이 준비되었는지 확인하는 로직 구현 
	//플레이어쪽에서 대기화면이 나타났을때 준비완료 버튼을 누를때마다 이벤트 함수 호출
	//그때마다 준비상태를 확인하고 4명이 준비완료되었을경우 게임 시작
	
	/*
	if (AllPlayerControllers.IsEmpty() == true)
	{
		return;
	}
	
	if (AllPlayerControllers[0]->bIsReady == true && 
		AllPlayerControllers[1]->bIsReady == true &&
		AllPlayerControllers[2]->bIsReady == true &&
		AllPlayerControllers[3]->bIsReady == true )
	{
		WaitingTimerStart();
	}
	*/
}


