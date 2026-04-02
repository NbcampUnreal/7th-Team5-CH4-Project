#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DDGameInstance.generated.h"

UCLASS()
class UDDGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 로비에서 참여자로 확정된 유저들의 명단 (맵 이동 시 유지)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	TArray<FString> ParticipantNames;
};
