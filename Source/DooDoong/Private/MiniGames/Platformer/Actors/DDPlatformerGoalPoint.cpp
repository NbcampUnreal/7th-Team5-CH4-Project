// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGames/Platformer/Actors/DDPlatformerGoalPoint.h"
#include "MiniGames/Platformer/GameMode/DDPlatformerGameMode.h"
#include "Common/DDLogManager.h"
#include "Components/BoxComponent.h"


ADDPlatformerGoalPoint::ADDPlatformerGoalPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(SceneRoot);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this ,&ADDPlatformerGoalPoint::OnComponentBeginOverlap);
}

void ADDPlatformerGoalPoint::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() == true)
	{
		ServerNotifyGoalReached();
	}
}

void ADDPlatformerGoalPoint::ServerNotifyGoalReached_Implementation()
{
	UE_LOG(LogPMJ, Log, TEXT("ServerFunctionCalled"));
}

void ADDPlatformerGoalPoint::OnComponentBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (HasAuthority() == true && OtherActor == nullptr)
	{
		return;
	}
	
	/* 게임모드 가져오기 */
	AGameModeBase* CurrentGameModeBase = Cast<AGameModeBase>(GetWorld()->GetAuthGameMode());
	if (CurrentGameModeBase != nullptr)
	{
		ADDPlatformerGameMode* PlatformerGameModeBase = Cast<ADDPlatformerGameMode>(CurrentGameModeBase);
		if (PlatformerGameModeBase != nullptr)
		{
			PlatformerGameModeBase->CheckGoalPlayerCharacter(OtherActor);
			UE_LOG(LogPMJ, Log, TEXT("점수 전달!"));
		}
	}
}


