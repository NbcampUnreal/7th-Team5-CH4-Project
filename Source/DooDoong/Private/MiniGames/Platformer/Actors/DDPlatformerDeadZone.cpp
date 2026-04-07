#include "MiniGames/Platformer/Actors/DDPlatformerDeadZone.h"

#include "Base/Player/DDBasePlayerState.h"
#include "MiniGames/Platformer/GameMode/DDPlatformerGameMode.h"
#include "Components/BoxComponent.h"
#include "Interfaces/IPluginManager.h"

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
	
	APawn* OverlapPawn = Cast<APawn>(OtherActor);
	if (IsValid(OverlapPawn) == true)
	{
		ADDBasePlayerState* DDPlayerState = OverlapPawn->GetPlayerState<ADDBasePlayerState>();
		if (DDPlayerState != nullptr)
		{
			ADDPlatformerGameMode* CurrentGameMode = Cast<ADDPlatformerGameMode>(GetWorld()->GetAuthGameMode());
			if (IsValid(CurrentGameMode) == true)
			{
				for (const TPair<int32, FPlatformerPlayerData>& EnteredPlayer : CurrentGameMode->PlayerDatas)
				{
					if (DDPlayerState->PlayerGameData.SlotIndex == EnteredPlayer.Value.PlayerSlotIndex)
					{
						OtherActor->SetActorLocation(EnteredPlayer.Value.SavePointLocation);
					}
				}
			}
		}
	}
}



