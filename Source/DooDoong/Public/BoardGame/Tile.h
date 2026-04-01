#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "TileData.h"
#include "Tile.generated.h"

UCLASS()
class DOODOONG_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	ATile();

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
	UPROPERTY(VisibleAnywhere, Category="Tile")
	FTileRowData TileData;

	UPROPERTY(EditAnywhere, Category="Tile")
	TArray<ATile*> NextTiles;

	FVector GetStandLocation() const;
	
	void LoadTileData();
	
	void ResolveNextTiles(const TMap<FName, ATile*>& TileMap);
};