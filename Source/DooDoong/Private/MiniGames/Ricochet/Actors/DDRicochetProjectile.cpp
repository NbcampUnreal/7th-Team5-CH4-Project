#include "MiniGames/Ricochet/Actors/DDRicochetProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MiniGames/Ricochet/Actors/DDRicochetTarget.h"

ADDRicochetProjectile::ADDRicochetProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(false);

	// Collision
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	SetRootComponent(CollisionComp);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CollisionComp->SetGenerateOverlapEvents(true);

	// Mesh
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Movement
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));

	ProjectileMovementComp->InitialSpeed = StartSpeed;
	ProjectileMovementComp->MaxSpeed = MaxSpeed;
	ProjectileMovementComp->ProjectileGravityScale = GravityScale;

	ProjectileMovementComp->bRotationFollowsVelocity = true;

	// ✔ 핵심 bounce 설정
	ProjectileMovementComp->bShouldBounce = true;
	ProjectileMovementComp->Bounciness = 0.6f;
	ProjectileMovementComp->Friction = 0.2f;
	ProjectileMovementComp->BounceVelocityStopSimulatingThreshold = 10.f;
}

void ADDRicochetProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSeconds);
}

void ADDRicochetProjectile::InitializeProjectile(AActor* InOwnerActor, APawn* InInstigatorPawn, const FVector& ShotDirection)
{
	SetOwner(InOwnerActor);
	SetInstigator(InInstigatorPawn);

	if (InInstigatorPawn)
	{
		CollisionComp->IgnoreActorWhenMoving(InInstigatorPawn, true);
	}

	const FVector Velocity = ShotDirection.GetSafeNormal() * StartSpeed;
	ProjectileMovementComp->Velocity = Velocity;
}

APlayerState* ADDRicochetProjectile::GetThrowerPlayerState() const
{
	const APawn* Pawn = GetInstigator();
	if (!Pawn) return nullptr;

	if (AController* Controller = Pawn->GetController())
	{
		return Controller->PlayerState;
	}

	return nullptr;
}

void ADDRicochetProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!HasAuthority() || !OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	if (ADDRicochetTarget* Target = Cast<ADDRicochetTarget>(OtherActor))
	{
		Target->HandleProjectileHit(this);
		Destroy();
	}
}