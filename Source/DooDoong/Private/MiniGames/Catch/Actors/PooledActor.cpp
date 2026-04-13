#include "MiniGames/Catch/Actors/PooledActor.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

APooledActor::APooledActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bInUse = true;

	// Root
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComp);

	// Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionComp);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		if (!InUse)
		{
			// 비활성화 시 맵 밖으로 이동
			SetActorLocation(PoolHideLocation, false, nullptr, ETeleportType::TeleportPhysics);
		}
		OnRep_InUse();
	}
}

bool APooledActor::IsInUse() const
{
	return bInUse;
}

void APooledActor::OnRep_InUse()
{
	// Hidden만 처리, 콜리전 건드리지 않음
	SetActorHiddenInGame(!bInUse);

	// 클라이언트도 비활성 시 맵 밖으로
	if (!bInUse)
	{
		SetActorLocation(PoolHideLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}
