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
	/* 캐릭터 고유 ID 생길경우 TMap 으로 리펙토링 예정 */
	ADDBasePlayerController* DDPlayerController = Cast<ADDBasePlayerController>(NewPlayer);
	if (IsValid(DDPlayerController) == true)
	{
		DDAllPlayerControllers.Add(DDPlayerController);
		ADDBaseCharacter* DDPlayerCharacter = Cast<ADDBaseCharacter>(DDPlayerController->GetPawn());
		if (IsValid(DDPlayerCharacter) == true)
		{
			DDAllPlayerCharacters.Add(DDPlayerCharacter);
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
		DDPlayerMaxDistance[i] = 0.0f;
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
		&ADDPlatformerGameModeBase::PlayeGameTimer,
		1.f,
		true);
}

void ADDPlatformerGameModeBase::GameEnd()
{
	UE_LOG(LogPMJ, Log, TEXT("GameEnd"));
	GetWorldTimerManager().ClearTimer(CheckPlayerMovedDistanceTimerHandle);
	//TODO_@Minjae : 점수 계산해서 넘겨주는 로직 구현
	
	PlayerRanking();
	
}

void ADDPlatformerGameModeBase::PlayeGameTimer()
{
	UE_LOG(LogPMJ, Log, TEXT("PlayeGameTimer"));
	
	for (int i = 0; i < DDAllPlayerCharacters.Num(); i++)
	{
		float CurrentPlayerDistance = FVector::Dist(StartLocation, DDAllPlayerCharacters[i]->GetActorLocation());
		if (DDPlayerMaxDistance[i] < CurrentPlayerDistance)
		{
			DDPlayerMaxDistance[i] = CurrentPlayerDistance;
		}
	}
}

void ADDPlatformerGameModeBase::CheckGoalPlayerCharacter(AActor* GoalActor)
{
	/* 일단은 간략하게 순위정하는거 구현해놓을건데 플레이어별로 구분할 수 있는 태그만들어서 나중에 리펙토링할게요 */
	for (int i = 0; i < DDAllPlayerCharacters.Num(); i++)
	{
		if (GoalActor->GetOwner() == DDAllPlayerCharacters[i])
		{
			PlayerGoalInArray.Add(DDAllPlayerCharacters[i]);
			DDAllPlayerCharacters.RemoveAt(i);
			DDPlayerMaxDistance.Remove(i);
		}
	}
}

void ADDPlatformerGameModeBase::PlayerRanking()
{
	UE_LOG(LogPMJ, Log, TEXT("PlayerRanking"));
	for (int i = 0; i < PlayerGoalInArray.Num(); i++)
	{
		PlayerRankingArray.Add(PlayerGoalInArray[i]);
	}
	
	if (PlayerRankingArray.Num() != DDAllPlayerControllers.Num())
	{
		int32 Index = DDAllPlayerCharacters.Num();
		for (int i = 0; i < Index; i++)
		{
			float MaxValue = DDPlayerMaxDistance[i];
			for (int j = 0; j < DDPlayerMaxDistance.Num(); j++)
			{
				if (MaxValue < DDPlayerMaxDistance[j])
				{
					MaxValue = DDPlayerMaxDistance[j];
				}
			}
			
			for (int j = 0; j < DDPlayerMaxDistance.Num(); j++)
			{
				if (MaxValue == DDPlayerMaxDistance[j])
				{
					PlayerRankingArray.Add(DDAllPlayerCharacters[j]);
					DDAllPlayerCharacters.RemoveAt(j);
					DDPlayerMaxDistance.RemoveAt(j);
				}
			}
		}
	}
	
	//TODO_@Minjae : AddScore 로직 필요
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


