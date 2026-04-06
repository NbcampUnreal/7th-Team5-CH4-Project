#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DDGameInstance.generated.h"

UCLASS()
class DOODOONG_API UDDGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    // ==========================================
    // 글로벌 장기 데이터 (맵 전환 시에도 유지됨)
    // ==========================================

    /** 현재 보드판 라운드 진행도 */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Global Data")
    int32 CurrentRound = 1;
};
