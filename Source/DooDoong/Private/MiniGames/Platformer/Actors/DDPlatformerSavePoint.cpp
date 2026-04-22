#include "MiniGames/Platformer/Actors/DDPlatformerSavePoint.h"
#include "MiniGames/Platformer/GameMode/DDPlatformerGameMode.h"
#include "Components/BoxComponent.h"

ADDPlatformerSavePoint::ADDPlatformerSavePoint()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(SceneRoot);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this ,&ADDPlatformerSavePoint::OnComponentBeginOverlap);
}

void ADDPlatformerSavePoint::OnComponentBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	
	if (OtherActor == nullptr) return;
	
	ADDPlatformerGameMode* CurrentGameMode = Cast<ADDPlatformerGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(CurrentGameMode))
	{
		CurrentGameMode->UpdatePlayerSavePoint(OtherActor, GetActorLocation());
	}
}



