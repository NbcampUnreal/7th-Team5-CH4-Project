#pragma once

#include "Engine/DataTable.h"
#include "DDTileData.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
	Normal,
	Start,
	Goal,
	Move,
	Coin,
	Item,
	Damage,
}; // 기본, 시작, 목표, 이동, 코인, 아이템, 데미지 

USTRUCT(BlueprintType)
struct FTileRowData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	ETileType TileType = ETileType::Normal;
	UPROPERTY(EditAnywhere)
	int32 Effect = 0;

	UPROPERTY(EditAnywhere)
	TArray<FName> NextTiles;
};
