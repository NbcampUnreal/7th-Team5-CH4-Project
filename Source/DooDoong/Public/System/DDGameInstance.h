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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Global Data", meta=(DisplayName = "현재 보드판 라운드 진행도"))
	int32 CurrentRound = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global Data", meta=(DisplayName= "우승에 필요한 목표 트로피 개수"))
	int32 MaxTrophy = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global Data", meta=(DisplayName = "게임이 종료되는 최대 라운드 수"))
	int32 MaxRound = 10;
};
