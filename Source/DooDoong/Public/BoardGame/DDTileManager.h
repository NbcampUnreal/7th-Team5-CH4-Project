#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDTileManager.generated.h"

class ADDTile;
UCLASS()
class ADDTileManager : public AActor
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:

	UPROPERTY()
	TMap<FName, ADDTile*> TileMap;

	void InitializeTiles();
};