#include "Base/Game/DDGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "BoardGame/DDTileManager.h"
#include "Kismet/GameplayStatics.h"

void ADDGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	TileManager = Cast<ADDTileManager>(
	UGameplayStatics::GetActorOfClass(GetWorld(), ADDTileManager::StaticClass())
);
}

void ADDGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADDGameStateBase, MatchState);
}
