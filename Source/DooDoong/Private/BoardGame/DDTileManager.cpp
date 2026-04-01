#include "BoardGame/DDTileManager.h"
#include "EngineUtils.h"
#include "BoardGame/DDTile.h"
#include "Common/DDLogManager.h"


void ADDTileManager::BeginPlay()
{
	Super::BeginPlay();

	InitializeTiles();
}

void ADDTileManager::InitializeTiles()
{
	// TileMap 구성
	for (TActorIterator<ADDTile> It(GetWorld()); It; ++It)
	{
		ADDTile* Tile = *It;
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
	LOG_CYS(Warning, TEXT("[TM]TileMap loaded. Good!"));
}