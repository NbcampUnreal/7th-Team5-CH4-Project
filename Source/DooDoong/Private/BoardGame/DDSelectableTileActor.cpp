#include "BoardGame/DDSelectableTileActor.h"
#include "Common/DDLogManager.h"

ADDSelectableTileActor::ADDSelectableTileActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 클릭

	bReplicates = true;
	TargetTile = nullptr;
}

void ADDSelectableTileActor::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicateMovement(true);
	LOG_CYS(Error, TEXT("화살표 생성"));
}
