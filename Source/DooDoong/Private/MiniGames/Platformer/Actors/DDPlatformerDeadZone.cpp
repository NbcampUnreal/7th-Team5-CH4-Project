#include "MiniGames/Platformer/Actors/DDPlatformerDeadZone.h"

#include "Base/Player/DDBasePlayerState.h"
#include "Common/DDLogManager.h"
#include "MiniGames/Platformer/GameMode/DDPlatformerGameMode.h"
#include "Components/BoxComponent.h"
#include "Interfaces/IPluginManager.h"

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
	if (OtherActor == nullptr)
	{
		return;
	}
	
	APawn* OverlapPawn = Cast<APawn>(OtherActor);
	if (IsValid(OverlapPawn) == false)
	{
		LOG_PMJ(Error, TEXT("=== DEADZONE : 폰 캐스팅 실패 ==="));
		return;
	}
	
	ADDBasePlayerState* DDPlayerState = OverlapPawn->GetPlayerState<ADDBasePlayerState>();
	if (DDPlayerState == nullptr)
	{
		LOG_PMJ(Error, TEXT("=== DEADZONE : 플레이어 스테이트 캐스팅 실패 ==="));
		return;
	}
	
	ADDPlatformerGameMode* CurrentGameMode = Cast<ADDPlatformerGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(CurrentGameMode) == false)
	{
		LOG_PMJ(Error, TEXT("=== DEADZONE : 게임모드 캐스팅 실패 ==="));
		return;
	}
	
	for (const TPair<int32, FPlatformerPlayerData>& EnteredPlayer : CurrentGameMode->PlayerDatas)
	{
		if (DDPlayerState->PlayerGameData.SlotIndex == EnteredPlayer.Value.PlayerSlotIndex)
		{
			OtherActor->SetActorLocation(EnteredPlayer.Value.SavePointLocation);
		}
	}
	
}



