//DDRicochetTarget.cpp
#include "MiniGames/Ricochet/Actors/DDRicochetTarget.h"

#include "Base/Player/DDBasePlayerState.h"
#include "Common/DDLogManager.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MiniGames/Ricochet/Actors/DDRicochetProjectile.h"
#include "MiniGames/Ricochet/GameMode/DDRicochetGameMode.h"

ADDRicochetTarget::ADDRicochetTarget()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	SetRootComponent(CollisionComp);
	CollisionComp->SetBoxExtent(FVector(50.0f));
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = StartSpeed;
	ProjectileMovementComp->MaxSpeed = MaxSpeed;
	ProjectileMovementComp->ProjectileGravityScale = GravityScale;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bShouldBounce = true;
}

void ADDRicochetTarget::BeginPlay()
{
	Super::BeginPlay();
}

void ADDRicochetTarget::ThrowTarget(const FVector& ThrowDirection, float Speed)
{
	const FVector ThrowVelocity = ThrowDirection.GetSafeNormal() * Speed;

	ProjectileMovementComp->Velocity = ThrowVelocity;
	ProjectileMovementComp->Activate(true);
}

bool ADDRicochetTarget::HandleProjectileHit(ADDRicochetProjectile* RicochetProjectile)
{
	if (!HasAuthority() || !bCanGetScore || RicochetProjectile == nullptr)
	{
		return false;
	}

	APlayerState* ThrowerPlayerState = RicochetProjectile->GetThrowerPlayerState();
	ADDRicochetGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ADDRicochetGameMode>() : nullptr;

	if (ThrowerPlayerState == nullptr || GameMode == nullptr)
	{
		return false;
	}

	bCanGetScore = false;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GameMode->AddScore(ThrowerPlayerState, ScoreValue);
	Destroy();

	const ADDBasePlayerState* DDPlayerState = Cast<ADDBasePlayerState>(ThrowerPlayerState);
	const FName DisplayName = DDPlayerState != nullptr
		? DDPlayerState->PlayerGameData.PlayerDisplayName
		: NAME_None;

	LOG_JJH(Warning, TEXT("Player : %s, Score : %d"),
		*DisplayName.ToString(),
		ScoreValue);

	return true;
}