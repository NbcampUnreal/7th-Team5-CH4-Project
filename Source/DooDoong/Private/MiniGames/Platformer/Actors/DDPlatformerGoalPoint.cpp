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
}

void ADDPlatformerGoalPoint::OnComponentBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	
	if (OtherActor == nullptr) return;
	
	ADDPlatformerGameMode* CurrentGameMode = Cast<ADDPlatformerGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(CurrentGameMode))
	{
		CurrentGameMode->CheckGoalPlayerCharacter(OtherActor);
	}
	
}


