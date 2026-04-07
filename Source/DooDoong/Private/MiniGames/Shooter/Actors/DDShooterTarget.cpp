// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGames/Shooter/Actors/DDShooterTarget.h"


// Sets default values
ADDShooterTarget::ADDShooterTarget()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADDShooterTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADDShooterTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

