#include "MiniGames/Catch/Actors/PooledActor.h"


APooledActor::APooledActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void APooledActor::SetInUse(bool InUse)
{
	bInUse = InUse;
	// 콜리전
	SetActorEnableCollision(InUse);
	// 숨김
	SetActorHiddenInGame(!InUse);
}

bool APooledActor::IsInUse() const
{
	return bInUse;
}

void APooledActor::BeginPlay()
{
	Super::BeginPlay();
	SetInUse(false);
}


