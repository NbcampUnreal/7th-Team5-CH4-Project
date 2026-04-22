#include "MiniGames/Platformer/Actors/DDPlatformerDeadZone.h"

#include "MiniGames/Platformer/GameMode/DDPlatformerGameMode.h"
#include "Components/BoxComponent.h"

ADDPlatformerDeadZone::ADDPlatformerDeadZone()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this ,&ADDPlatformerDeadZone::OnComponentBeginOverlap);
}

void ADDPlatformerDeadZone::BeginPlay()
{
	Super::BeginPlay();
}

void ADDPlatformerDeadZone::OnComponentBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
	)
{
	if (!HasAuthority()) return;
	
	if (OtherActor == nullptr) return;
	
	ADDPlatformerGameMode* CurrentGameMode = Cast<ADDPlatformerGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(CurrentGameMode))
	{
		CurrentGameMode->ReturnToSavePoint(OtherActor);
	}
}



