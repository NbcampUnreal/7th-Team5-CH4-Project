#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TestLogicManager.generated.h"

USTRUCT(BlueprintType)
struct FTestPlayerData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString Name = TEXT("");

    UPROPERTY(BlueprintReadWrite)
    int32 Gold = 0;

    UPROPERTY(BlueprintReadWrite)
    int32 Health = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* ProfileImage = nullptr;
};

UCLASS(Blueprintable)
class DOODOONG_API UTestLogicManager : public UObject
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadWrite)
    TArray<FTestPlayerData> Players;

    UFUNCTION(BlueprintCallable)
    void InitTestPlayers();

    UFUNCTION(BlueprintCallable)
    void GiveGold(int32 PlayerIndex, int32 Amount);

    UFUNCTION(BlueprintCallable)
    void GiveHealth(int32 PlayerIndex, int32 Amount);

    UFUNCTION(BlueprintCallable)
    TArray<FTestPlayerData> GetSortedPlayers();
};

