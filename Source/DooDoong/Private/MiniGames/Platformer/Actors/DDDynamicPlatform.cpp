// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGames/Platformer/Actors/DDDynamicPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Common/DDLogManager.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"

ADDDynamicPlatform::ADDDynamicPlatform()
	: TimerRate(0.016f)
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(RootScene);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MeshComponent->SetupAttachment(RootScene);
}

void ADDDynamicPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();
	MoveDirection = PlatformVelocity.GetSafeNormal();
	
	if (PlatformVelocity != FVector::ZeroVector)
	{
		GetWorldTimerManager().SetTimer(
			DynamicPlatformTimerHandle,
			this,
			&ADDDynamicPlatform::MovingPlatform,
			TimerRate,
			true
		);
	}
	else if (RotationValue != FRotator::ZeroRotator)
	{
		GetWorldTimerManager().SetTimer(
			DynamicPlatformTimerHandle,
			this,
			&ADDDynamicPlatform::RotatingPlatform,
			TimerRate,
			true
		);
	}
}

void ADDDynamicPlatform::MovingPlatform()
{
	UE_LOG(LogPMJ, Log, TEXT("MovingPlatform"))
	FVector CurrentLocation = GetActorLocation();
	CurrentLocation += MoveDirection * PlatformVelocity * TimerRate;
	SetActorLocation(CurrentLocation);
	float DistanceMoved = FVector::Dist(StartLocation, CurrentLocation);
	
	if (DistanceMoved > MoveDistance)
	{
		StartLocation = StartLocation + MoveDistance * MoveDirection;
		SetActorLocation(StartLocation);
		MoveDirection *= -1;
	}
}

void ADDDynamicPlatform::RotatingPlatform()
{
	FRotator CurrentRotation = GetActorRotation();
	AddActorLocalRotation(RotationValue * TimerRate);
}


