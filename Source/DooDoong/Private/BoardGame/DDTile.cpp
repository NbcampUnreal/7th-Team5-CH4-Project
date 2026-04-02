#include "BoardGame/DDTile.h"
#include "Common/DDLogManager.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

ADDTile::ADDTile()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Mesh
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetupAttachment(Root);

	// Stand Point
	StandPoint = CreateDefaultSubobject<USceneComponent>(TEXT("StandPoint"));
	StandPoint->SetupAttachment(Root);
}

void ADDTile::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector ADDTile::GetStandLocation(ACharacter* Character) const
{
	if (!StandPoint) return GetActorLocation();

	FVector Location = StandPoint->GetComponentLocation();

	if (Character)
	{
		float HalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		Location.Z += HalfHeight;
	}

	return Location;
}

void ADDTile::LoadTileData()
{
	if (!TileDataTable || TileRowName.IsNone())
	{
		LOG_CYS(Error, TEXT("[Tile]TileDataTable or RowName missing"));
		return;
	}

	FTileRowData* Row = TileDataTable->FindRow<FTileRowData>(TileRowName, TEXT(""));

	if (Row)
	{
		TileData = *Row;
	}
	else
	{
		LOG_CYS(Error, TEXT("[Tile]Row not found: %s"), *TileRowName.ToString());
	}
}

void ADDTile::ResolveNextTiles(const TMap<FName, ADDTile*>& TileMap)
{
	// 기존 연결 초기화
	NextTiles.Empty();

	// DataTable에 NextTiles 없으면 종료
	if (TileData.NextTiles.Num() == 0)
	{
		return;
	}

	for (const FName& NextName : TileData.NextTiles)
	{
		if (NextName.IsNone())
		{
			continue;
		}

		ADDTile* FoundTile = TileMap.FindRef(NextName);

		if (FoundTile)
		{
			NextTiles.Add(FoundTile);
		}
		else
		{
			LOG_CYS(Error, TEXT("[Tile][%s] NextTile not found: %s"),
				*TileRowName.ToString(),
				*NextName.ToString());
		}
	}
}
