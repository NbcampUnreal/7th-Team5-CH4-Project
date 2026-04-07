#include "MiniGames/Platformer/Actors/DDPlatformerSavePoint.h"
#include "MiniGames/Platformer/GameMode/DDPlatformerGameMode.h"
#include "Base/Player/DDBasePlayerState.h"
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
	if (OtherActor == nullptr)
	{
		return;
	}
	
	APawn* OverlapPawn = Cast<APawn>(OtherActor);
	if (IsValid(OverlapPawn) == true)
	{
		ADDBasePlayerState* DDPlayerState = OverlapPawn->GetPlayerState<ADDBasePlayerState>();
		if (DDPlayerState != nullptr)
		{
			ADDPlatformerGameMode* CurrentGameMode = Cast<ADDPlatformerGameMode>(GetWorld()->GetAuthGameMode());
			if (IsValid(CurrentGameMode) == true)
			{
				for (TPair<int32, FPlatformerPlayerData>& EnteredPlayer : CurrentGameMode->PlayerDatas)
				{
					if (DDPlayerState->PlayerGameData.SlotIndex == EnteredPlayer.Value.PlayerSlotIndex)
					{
						EnteredPlayer.Value.SavePointLocation = GetActorLocation();
					}
				}
			}
		}
	}
}



