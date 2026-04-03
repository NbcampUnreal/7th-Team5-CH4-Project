// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGames/Platformer/Actors/DDPlatformerGoalPoint.h"
#include "MiniGames/Platformer/GameMode/DDPlatformerGameModeBase.h"
#include "Common/DDLogManager.h"

#include "Components/BoxComponent.h"
#include "Interfaces/IPluginManager.h"


ADDPlatformerGoalPoint::ADDPlatformerGoalPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(SceneRoot);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this ,&ADDPlatformerGoalPoint::OnComponentBeginOverlap);
}

void ADDPlatformerGoalPoint::OnComponentBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)
	{
		return;
	}
	
	/*APawn* PlayerPawn = Cast<APawn>(OtherActor->GetOwner());
	if (PlayerPawn != nullptr)
	{
		APlayerState* PlayerState = PlayerPawn->GetPlayerState();
	}*/
	
	/* 게임모드 가져오기 */
	AGameModeBase* CurrentGameModeBase = Cast<AGameModeBase>(GetWorld()->GetAuthGameMode());
	if (CurrentGameModeBase != nullptr)
	{
		ADDPlatformerGameModeBase* DDPlatformerGameModeBase = Cast<ADDPlatformerGameModeBase>(CurrentGameModeBase);
		if (DDPlatformerGameModeBase != nullptr)
		{
			DDPlatformerGameModeBase->CheckGoalPlayerCharacter(OtherActor);
			//TODO_@Minjae : 점수전달 로직 구현
			UE_LOG(LogPMJ, Log, TEXT("점수 전달!"));
		}
	}
}


