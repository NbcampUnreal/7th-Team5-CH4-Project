#include "BoardGame/Tile.h"
#include "Common/LogManager.h"

ATile::ATile()
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

void ATile::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector ATile::GetStandLocation() const
{
	return StandPoint->GetComponentLocation();
}

void ATile::LoadTileData()
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

void ATile::ResolveNextTiles(const TMap<FName, ATile*>& TileMap)
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

		ATile* FoundTile = TileMap.FindRef(NextName);

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
