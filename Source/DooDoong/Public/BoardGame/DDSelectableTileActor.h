#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDSelectableTileActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileSelected, ADDTile*, SelectedTile);

class ADDTile;

UCLASS()
class DOODOONG_API ADDSelectableTileActor : public AActor
{
	GENERATED_BODY()

public:
	ADDSelectableTileActor();
	
	UPROPERTY(BlueprintAssignable)
	FOnTileSelected OnTileSelected;
	
	ADDTile* TargetTile;
private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
};
