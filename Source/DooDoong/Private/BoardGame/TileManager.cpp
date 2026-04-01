#include "BoardGame/TileManager.h"
#include "EngineUtils.h"
#include "BoardGame/Tile.h"
#include "Common/LogManager.h"


void ATileManager::BeginPlay()
{
	Super::BeginPlay();

	InitializeTiles();
}

void ATileManager::InitializeTiles()
{
	// TileMap 구성
	for (TActorIterator<ATile> It(GetWorld()); It; ++It)
	{
		ATile* Tile = *It;
		// 이름없는 타일 체크
		if (Tile->TileRowName.IsNone())
		{
			LOG_CYS(Error, TEXT("[TM]TileRowName not set on %s"), *Tile->GetName());
			continue;
		}
		
		// 중복 타일 체크
		if (TileMap.Contains(Tile->TileRowName))
		{
			LOG_CYS(Error, TEXT("[TM]Duplicate TileRowName: %s"), *Tile->TileRowName.ToString());
			continue;
		}
		
		TileMap.Add(Tile->TileRowName, Tile);
	}

	// DataTable 로드
	for (auto& Pair : TileMap)
	{
		Pair.Value->LoadTileData();
	}

	// NextTiles 연결
	for (auto& Pair : TileMap)
	{
		Pair.Value->ResolveNextTiles(TileMap);
	}
}