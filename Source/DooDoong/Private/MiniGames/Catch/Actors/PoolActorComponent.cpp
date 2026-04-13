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
		LOG_CYS(Error, TEXT("[Pool] PoolSize: %d"), PoolSize);
	}
}

void UPoolActorComponent::InitializePool()
{
	if (!PooledActorClass) return;
	LOG_CYS(Error, TEXT("[Pool] InitializePool 시작"));
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
		if (!PooledActor)
		{
			LOG_CYS(Error, TEXT("[Pool] SpawnActor 실패!!! Class: %s"), *GetNameSafe(PooledActorClass));
		}
		else
		{
			LOG_CYS(Error, TEXT("[Pool] Spawn 성공: %s"), *PooledActor->GetName());
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

APooledActor* UPoolActorComponent::SpawnFromPool(const FTransform& SpawnTransform)
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
		LOG_CYS(Error,TEXT("풀 없음"));
		return nullptr; // 없으면 그냥 종료
	}

	// 재사용 (스폰 대신)
	Actor->SetActorTransform(SpawnTransform);
	Actor->SetInUse(true);

	return Actor;
}
