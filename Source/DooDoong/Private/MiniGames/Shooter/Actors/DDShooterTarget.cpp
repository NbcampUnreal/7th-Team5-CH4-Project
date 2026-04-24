#include "MiniGames/Shooter/Actors/DDShooterTarget.h"

#include "Base/Player/DDBasePlayerState.h"
#include "Common/DDLogManager.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MiniGames/Shooter/Actors/DDShotProjectile.h"
#include "MiniGames/Shooter/GameMode/DDShooterGameMode.h"
#include "System/DDSoundManager.h"

ADDShooterTarget::ADDShooterTarget()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

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

void ADDShooterTarget::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicateMovement(true);
}

void ADDShooterTarget::LaunchTarget(const FVector& LaunchDirection, float Speed)
{
	const FVector LaunchVelocity = LaunchDirection.GetSafeNormal() * Speed;
	
	ProjectileMovementComp->Velocity = LaunchVelocity;
	ProjectileMovementComp->Activate(true);
}

bool ADDShooterTarget::HandleProjectileHit(ADDShotProjectile* ShotProjectile)
{
	if (!HasAuthority() || !bCanGetScore || ShotProjectile == nullptr)
	{
		return false;
	}

	APlayerState* ShooterPlayerState = ShotProjectile->GetShooterPlayerState();
	ADDShooterGameMode* ShooterGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ADDShooterGameMode>() : nullptr;
	if (ShooterPlayerState == nullptr || ShooterGameMode == nullptr)
	{
		return false;
	}

	bCanGetScore = false;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShooterGameMode->AddScore(ShooterPlayerState, ScoreValue);
	
	Destroy();

	const ADDBasePlayerState* ShooterDDPlayerState = Cast<ADDBasePlayerState>(ShooterPlayerState);
	const FName ShooterDisplayName = ShooterDDPlayerState != nullptr
		                                 ? ShooterDDPlayerState->PlayerGameData.PlayerDisplayName
		                                 : NAME_None;
	
	LOG_JJH(Warning, TEXT("플레이어 : %s, 획득 점수 : %d"),
	        *ShooterDisplayName.ToString(),
	        ScoreValue);
	return true;
}
