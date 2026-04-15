#pragma once

#include "CoreMinimal.h"
#include "Base/Game/DDGameStateBase.h"
#include "DDBoardGameState.generated.h"

UCLASS()
class DOODOONG_API ADDBoardGameState : public ADDGameStateBase 
{
    GENERATED_BODY()

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
public:
    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "GameData", meta=(DisplayName="각 턴당 주어지는 최대 제한 시간"))
    int32 MaxStateTimer = 30;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "GameData",  meta=(DisplayName="현재 상태에서 남은 제한 시간"))
    int32 StateTimer = 0;
    
    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "GameData", meta=(DisplayName="우승에 필요한 목표 트로피 개수"))
    int32 MaxTrophy = 1;

    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "GameData", meta=(DisplayName="게임이 종료되는 최대 라운드 수"))
    int32 MaxRound = 10;
};