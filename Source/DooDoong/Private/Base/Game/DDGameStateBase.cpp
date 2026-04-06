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

	// 태그를 서버에서 클라이언트로 완벽하게 동기화합니다.
	DOREPLIFETIME(ADDGameStateBase, MatchStateTag);
}
