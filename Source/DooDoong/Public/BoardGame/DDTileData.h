#pragma once

#include "Engine/DataTable.h"
#include "DDTileData.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8 {
	Normal,
	Start, 
	Goal,
	Move, 
	Coin, 
	Item, 
}; // 기본, 시작, 목표, 이동, 코인, 아이템 

USTRUCT(BlueprintType)
struct FTileRowData : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) ETileType TileType;
	UPROPERTY(EditAnywhere) int32 MoveEffect;
	UPROPERTY(EditAnywhere) int32 CoinEffect;

	UPROPERTY(EditAnywhere)
	TArray<FName> NextTiles;
};