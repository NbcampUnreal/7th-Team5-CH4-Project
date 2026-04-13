#include "MiniGames/Catch/Actors/PooledActor.h"

#include "Net/UnrealNetwork.h"


APooledActor::APooledActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bInUse=true;
	// Root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

void APooledActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APooledActor, bInUse);
}

void APooledActor::SetInUse(bool InUse)
{
	if (bInUse == InUse) return;

	bInUse = InUse;

	if (HasAuthority())
	{
		OnRep_InUse(); // 서버에서도 직접 적용
	}
}

bool APooledActor::IsInUse() const
{
	return bInUse;
}

void APooledActor::BeginPlay()
{
	Super::BeginPlay();
}

void APooledActor::OnRep_InUse()
{
	SetActorEnableCollision(bInUse);
	SetActorHiddenInGame(!bInUse);
}


