// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGames/Platformer/Actors/DDDynamicPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Common/DDLogManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"

ADDDynamicPlatform::ADDDynamicPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(RootScene);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MeshComponent->SetupAttachment(RootScene);
	
}

void ADDDynamicPlatform::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	MovingPlatform(DeltaSeconds);
}

void ADDDynamicPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();
	MoveDirection = PlatformVelocity.GetSafeNormal();
}

void ADDDynamicPlatform::MovingPlatform(float DeltaSeconds)
{
	FVector CurrentLocation = GetActorLocation();
	CurrentLocation += MoveDirection * (PlatformVelocity * DeltaSeconds);
	SetActorLocation(CurrentLocation);
	float DistanceMoved = FVector::Dist(StartLocation, CurrentLocation);
	
	if (DistanceMoved > MoveDistance)
	{
		StartLocation = StartLocation + MoveDistance * MoveDirection;
		SetActorLocation(StartLocation);
		MoveDirection *= -1;
	}
}

void ADDDynamicPlatform::RotatingPlatform(float DeltaSeconds)
{
	FRotator CurrentRotation = GetActorRotation();
	SetActorRelativeRotation(RotationValue * DeltaSeconds);
}


