#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "DDTileData.h"
#include "Character/DDBoardGameCharacter.h"
#include "DDTile.generated.h"

class ACharacter;

// USTRUCT(BlueprintType)
// struct FTileVisualData
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere)
// 	UMaterialInterface* Material;
//
// 	UPROPERTY(EditAnywhere)
// 	UTexture2D* Icon;
// };

UCLASS()
class DOODOONG_API ADDTile : public AActor
{
	GENERATED_BODY()
	
public:	
	ADDTile();

protected:
	virtual void BeginPlay() override;

public:

	// 컴포넌트
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TileMesh;

	// 캐릭터가 서는 위치
	UPROPERTY(VisibleAnywhere)
	USceneComponent* StandPoint;
	
	// DataTable 참조
	UPROPERTY(EditAnywhere, Category="Tile")
	UDataTable* TileDataTable;

	// RowName (현재 타일 ID)
	UPROPERTY(EditAnywhere, Category="Tile")
	FName TileRowName;

	// 실제 데이터
	UPROPERTY(ReplicatedUsing = OnRep_TileData)
	FTileRowData TileData;

	UPROPERTY(EditAnywhere, Category="Tile")
	TArray<ADDTile*> NextTiles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile")
	TMap<ETileType, UMaterialInterface*> TileMaterialMap;
	// TMap<ETileType, FTileVisualData> TileVisualMap;
	
	FVector GetStandLocation(ACharacter* Character) const;
	
	void LoadTileData();
	
	void ResolveNextTiles(const TMap<FName, ADDTile*>& TileMap);
	
	void ApplyTileMaterial();
	
	UFUNCTION()
	void OnRep_TileData();
	void OnCharacterArrived(ADDBoardGameCharacter* Character) const;
	bool IsGoal() const;
};