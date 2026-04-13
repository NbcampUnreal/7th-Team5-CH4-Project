#include "MiniGames/Catch/Actors/PoolActorComponent.h"


UPoolActorComponent::UPoolActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UPoolActorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority()) // 서버에서만 생성
	{
		InitializePool();
	}
}

void UPoolActorComponent::InitializePool()
{
	if (!PooledActorClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	for (int32 i = 0; i < PoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APooledActor* PooledActor = World->SpawnActor<APooledActor>(
			PooledActorClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (PooledActor)
		{
			PooledActor->SetReplicates(true); // 클라이언트 복제
			PooledActor->SetInUse(false);     // 초기 비활성

			ObjectPool.Add(PooledActor);
		}
	}
}
APooledActor* UPoolActorComponent::FindFirstAvailableActor() const
{
	for (APooledActor* Actor : ObjectPool)
	{
		if (!Actor) continue;

		if (!Actor->IsInUse())
		{
			return Actor;
		}
	}

	return nullptr;
}

APooledActor* UPoolActorComponent::SpawnFromPool(const FTransform& SpawnTransform) const
{
	// 서버에서만 실행
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return nullptr;
	}

	// Find
	APooledActor* Actor = FindFirstAvailableActor();
	if (!Actor)
	{
		return nullptr; // 없으면 그냥 종료
	}

	// 재사용 (스폰 대신)
	Actor->SetActorTransform(SpawnTransform);
	Actor->SetInUse(true);

	return Actor;
}
