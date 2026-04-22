#include "BoardGame/DDTileManager.h"
#include "EngineUtils.h"
#include "BoardGame/DDTile.h"
#include "Common/DDLogManager.h"


void ADDTileManager::BeginPlay()
{
	Super::BeginPlay();
	
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
			LOG_CYS(Warning, TEXT("[TM]TileRowName not set on %s"), *Tile->GetName());
			continue;
		}
		
		// 중복 타일 체크
		if (TileMap.Contains(Tile->TileRowName))
		{
			LOG_CYS(Warning, TEXT("[TM]Duplicate TileRowName: %s"), *Tile->TileRowName.ToString());
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
	// 플레이어 state 로 캐릭터 시작 위치 이동 시키기
	
	LOG_CYS(Warning, TEXT("[TM]TileMap loaded. Good!"));
}

ADDTile* ADDTileManager::FindTile(FName TileName)
{
	if (ADDTile** FoundTile = TileMap.Find(TileName))
	{
		return *FoundTile;
	}
	return nullptr;
}
