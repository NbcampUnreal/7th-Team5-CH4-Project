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
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game State", meta=(DisplayName="게임 루프 상태"))
    FGameplayTag MatchStateTag;
	
	UPROPERTY()
	ADDTileManager* TileManager;
	
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Game State", meta=(DisplayName="게임 참가자 수"))
    int32 MinPlayerCount = 4;
};
