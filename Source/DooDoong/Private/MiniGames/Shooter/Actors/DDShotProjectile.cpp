#include "MiniGames/Shooter/Actors/DDShotProjectile.h"

#include "Base/Character/DDBaseCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MiniGames/Shooter/Actors/DDShooterTarget.h"

ADDShotProjectile::ADDShotProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	SetRootComponent(CollisionComp);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetGenerateOverlapEvents(true);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = StartSpeed;
	ProjectileMovementComp->MaxSpeed = MaxSpeed;
	ProjectileMovementComp->ProjectileGravityScale = GravityScale;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bShouldBounce = false;
}

void ADDShotProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSeconds);
	SetReplicateMovement(true);
}

void ADDShotProjectile::InitializeShot(AActor* InOwnerActor, APawn* InInstigatorPawn, const FVector& ShotDirection)
{
	SetOwner(InOwnerActor);
	SetInstigator(InInstigatorPawn);

	if (InInstigatorPawn != nullptr)
	{
		CollisionComp->IgnoreActorWhenMoving(InInstigatorPawn, true);
	}

	const FVector ShotVelocity = ShotDirection.GetSafeNormal() * StartSpeed;
	
	ProjectileMovementComp->Velocity = ShotVelocity;
}

APlayerState* ADDShotProjectile::GetShooterPlayerState() const
{
	const APawn* InstigatorPawn = GetInstigator();
	if (InstigatorPawn == nullptr)
	{
		return nullptr;
	}

	if (const AController* ShooterController = InstigatorPawn->GetController())
	{
		return ShooterController->PlayerState;
	}

	return nullptr;
}

void ADDShotProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!HasAuthority() || OtherActor == nullptr || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	if (APawn* ShooterPawn = GetInstigator())
	{
		if (ADDBaseCharacter* DDCharacter = Cast<ADDBaseCharacter>(ShooterPawn))
		{
			DDCharacter->Client_PlayLocalSound("SFX_HitBread");
		}
	}
	
	if (ADDShooterTarget* ShooterTarget = Cast<ADDShooterTarget>(OtherActor))
	{
		ShooterTarget->HandleProjectileHit(this);
		Destroy();
	}
}
