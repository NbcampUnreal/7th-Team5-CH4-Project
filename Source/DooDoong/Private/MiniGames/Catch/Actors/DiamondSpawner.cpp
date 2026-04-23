#include "MiniGames/Catch/Actors/DiamondSpawner.h"

#include "Common/DDLogManager.h"
#include "MiniGames/Catch/Actors/PoolActorComponent.h"
#include "Components/BoxComponent.h"
#include "MiniGames/Catch/Actors/PooledActor.h"


ADiamondSpawner::ADiamondSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	RootComponent = SpawnBox;

	SpawnBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bReplicates = true;
	
	Pool = CreateDefaultSubobject<UPoolActorComponent>(TEXT("Pool"));
}

void ADiamondSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void ADiamondSpawner::SpawnDiamonds()
{
	if (!HasAuthority() || !Pool) return;

	LOG_CYS(Warning, TEXT("[스포너] 다이아 스폰 시작"));
	int32 Spawned = 0;

	for (int32 i = 0; i < SpawnCount; i++)
	{
		FVector RandomLoc = GetRandomPointInBox();

		FTransform Transform;
		Transform.SetLocation(RandomLoc);

		APooledActor* Actor = Pool->SpawnFromPool(Transform);

		if (!Actor)
		{
			LOG_CYS(Warning, TEXT("[스포너] 풀 부족"));
			break; // 풀 부족
		}

		Spawned++;
	}
	LOG_CYS(Warning, TEXT("[스포너] Spawned %d Diamonds"), Spawned);
}

FVector ADiamondSpawner::GetRandomPointInBox() const
{
	FVector Origin = SpawnBox->GetComponentLocation();
	FVector Extent = SpawnBox->GetScaledBoxExtent();

	return Origin + FVector(
		FMath::RandRange(-Extent.X, Extent.X),
		FMath::RandRange(-Extent.Y, Extent.Y),
		FMath::RandRange(-Extent.Z, Extent.Z)
	);
}

void ADiamondSpawner::StartSpawn()
{
	if (!HasAuthority()) return;
	LOG_CYS(Warning, TEXT("[스포너] StartSpawn"));

	SpawnDiamonds();

	GetWorldTimerManager().SetTimer(
		SpawnTimer,
		this,
		&ADiamondSpawner::SpawnDiamonds,
		SpawnInterval,
		true
	);
}

void ADiamondSpawner::StopSpawn()
{
	GetWorldTimerManager().ClearTimer(SpawnTimer);
}
