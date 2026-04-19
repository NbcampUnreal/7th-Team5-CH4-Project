#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DDGameInstance.generated.h"

class UDataTable;

UCLASS()
class DOODOONG_API UDDGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// ==========================================
	// 글로벌 장기 데이터 (맵 전환 시에도 유지됨)
	// ==========================================

	/** 주현 : DDGameInstance Getter */
	static UDDGameInstance* Get(const UObject* WorldContext);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Global Data", meta=(DisplayName = "현재 보드판 라운드 진행도"))
	int32 CurrentRound = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<UDataTable> SoundDataTable;
	
	// 로비에 있던 총 인원 수
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
    int32 ExpectedPlayerCount = 0;
};
