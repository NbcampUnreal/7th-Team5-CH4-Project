#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameplayTagContainer.h"
#include "DDGameStateBase.generated.h"

class ADDTileManager;

UCLASS()
class DOODOONG_API ADDGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ADDGameStateBase();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY()
	ADDTileManager* TileManager;
	
public:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> LevelMainWidgetClass;
};
