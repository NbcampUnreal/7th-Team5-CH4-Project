#include "MiniGames/Platformer/Actors/DDPlatformerDeadZone.h"

#include "Components/BoxComponent.h"

ADDPlatformerDeadZone::ADDPlatformerDeadZone()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this ,&ADDPlatformerDeadZone::OnComponentBeginOverlap);
}

void ADDPlatformerDeadZone::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = FVector(0.0f, 0.0f, 0.0f);
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
	if (OtherActor == nullptr)
	{
		return;
	}
	
	OtherActor->SetActorLocation(StartLocation);
	
}



