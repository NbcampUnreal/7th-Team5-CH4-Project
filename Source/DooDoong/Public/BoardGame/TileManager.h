#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileManager.generated.h"

class ATile;
UCLASS()
class ATileManager : public AActor
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:

	UPROPERTY()
	TMap<FName, ATile*> TileMap;

	void InitializeTiles();
};