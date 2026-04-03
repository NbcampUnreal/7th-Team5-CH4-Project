// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGames/Platformer/GameMode/DDPlatformerGameModeBase.h"
#include "MiniGames/Platformer/GameState/DDPlatformerGameStateBase.h"
#include "Base/Character/DDBaseCharacter.h"
#include "Base/Player/DDBasePlayerController.h"
#include "Common/DDLogManager.h"
#include "System/DDGameplayTags.h"

void ADDPlatformerGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	ADDBasePlayerController* DDPlayerController = Cast<ADDBasePlayerController>(NewPlayer);
	if (IsValid(DDPlayerController) == true)
	{
		DDAllPlayerControllers.Add(DDPlayerController);
		for (int i = 0; i < DDAllPlayerControllers.Num(); i++)
		{
			ADDBaseCharacter* DDPlayerCharacter = Cast<ADDBaseCharacter>(DDAllPlayerControllers[i]->GetPawn());
			if (IsValid(DDPlayerCharacter) == true)
			{
				DDAllPlayerCharacters.Add(DDPlayerCharacter);
			}
		}
		//TODO_@Minjae : Setter 함수 완성되면 DA랑 IMC 넘겨주기
	}
}

void ADDPlatformerGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = FVector(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < DDAllPlayerCharacters.Num(); i++)
	{
		DDPlayerMaxDistance[i] = 0.f;
	}
	
	AGameStateBase* GameStateBase = Cast<AGameStateBase>(GetWorld()->GetGameState());
	if (IsValid(GameStateBase) == true)
	{
		PlatformerGameStateBase = Cast<ADDPlatformerGameStateBase>(GameStateBase);
	}
	
	WaitingTimerStart();
}

void ADDPlatformerGameModeBase::GameStart()
{
	UE_LOG(LogPMJ, Log, TEXT("GameStart"));
	PlatformerGameStateBase->SetMiniGameState(DDGameplayTags::State_MiniGame_Playing);
	GetWorldTimerManager().ClearTimer(FinishedWaitingTimerHandle);
	GetWorldTimerManager().SetTimer(
		PlatformerPlayTimerHandle,
		this,
		&ADDPlatformerGameModeBase::GameEnd,
		10.f,
		false,
		PlatformerPlayTime);
	
	GetWorldTimerManager().SetTimer(
		CheckPlayerMovedDistanceTimerHandle,
		this,
		&ADDPlatformerGameModeBase::CheckPlayerMovedDistanceTimer,
		1.f,
		true);
}

void ADDPlatformerGameModeBase::GameEnd()
{
	UE_LOG(LogPMJ, Log, TEXT("GameEnd"));
	GetWorldTimerManager().ClearTimer(CheckPlayerMovedDistanceTimerHandle);
	//TODO_@Minjae : 점수 계산해서 넘겨주는 로직 구현
}

void ADDPlatformerGameModeBase::CheckPlayerMovedDistanceTimer()
{
	UE_LOG(LogPMJ, Log, TEXT("CheckPlayerMovedDistanceTimer"));
	
	for (int i = 0; i < DDAllPlayerCharacters.Num(); i++)
	{
		float CurrentPlayerDistance = FVector::Dist(StartLocation, DDAllPlayerCharacters[i]->GetActorLocation());
		if (DDPlayerMaxDistance[i] < CurrentPlayerDistance)
		{
			DDPlayerMaxDistance[i] = CurrentPlayerDistance;
		}
	}
}

void ADDPlatformerGameModeBase::WaitingTimerStart()
{
	PlatformerGameStateBase->SetMiniGameState(DDGameplayTags::State_MiniGame_Preparing);
	UE_LOG(LogPMJ, Log, TEXT("WaitingTimerStart"));
	
	GetWorld()->GetTimerManager().SetTimer(
	FinishedWaitingTimerHandle,
	this,
	&ADDPlatformerGameModeBase::GameStart,
	10.f,
	false,
	10.f);
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


