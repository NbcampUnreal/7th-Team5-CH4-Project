#include "MiniGames/Catch/Actors/PoolActorComponent.h"
#include "Common/DDLogManager.h"

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
		LOG_CYS(Warning, TEXT("[Pool] PoolSize: %d"), PoolSize);
	}
}

void UPoolActorComponent::InitializePool()
{
	if (!PooledActorClass) return;
	LOG_CYS(Warning, TEXT("[Pool] InitializePool 시작"));
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
		else if (!PooledActor)
		{
			LOG_CYS(Warning, TEXT("[Pool] SpawnActor 실패!!! Class: %s"), *GetNameSafe(PooledActorClass));
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

APooledActor* UPoolActorComponent::SpawnFromPool(const FTransform& SpawnTransform)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return nullptr;

	APooledActor* Actor = FindFirstAvailableActor();
	if (!Actor) return nullptr;

	// 위치 먼저 텔레포트
	Actor->SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);

	// 활성화 (콜리전 토글 없음)
	Actor->SetInUse(true);

	return Actor;
}
