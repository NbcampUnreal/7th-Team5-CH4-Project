// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGames/Platformer/Actors/DDDynamicPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

#include "MovieSceneSequenceID.h"

ADDDynamicPlatform::ADDDynamicPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	
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
}

void ADDDynamicPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MovingPlatform(DeltaTime);
	RotatingPlatform(DeltaTime);
}

void ADDDynamicPlatform::MovingPlatform(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	CurrentLocation += MoveDirection * PlatformVelocity * DeltaTime;
	SetActorLocation(CurrentLocation);
	float DistanceMoved = FVector::Dist(StartLocation, CurrentLocation);
	
	if (DistanceMoved > MoveDistance)
	{
		StartLocation = StartLocation + MoveDistance * MoveDirection;
		SetActorLocation(StartLocation);
		MoveDirection *= -1;
	}
}

void ADDDynamicPlatform::RotatingPlatform(float DeltaTime)
{
	FRotator CurrentRotation = GetActorRotation();
	AddActorLocalRotation(RotationValue * DeltaTime);
}


