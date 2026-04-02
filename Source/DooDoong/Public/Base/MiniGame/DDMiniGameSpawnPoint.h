#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDMiniGameSpawnPoint.generated.h"

class UArrowComponent;
class USceneComponent;

UCLASS()
class DOODOONG_API ADDMiniGameSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	ADDMiniGameSpawnPoint();

public:
	/** 미니게임 참가자 SlotIndex 정보와 매칭되는 번호 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="MiniGame")
	int32 SlotIndex = INDEX_NONE;

protected:
	/** 레벨 배치용 루트 컴포넌트 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	/** 스폰 방향을 확인하기 위한 화살표 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	TObjectPtr<UArrowComponent> ArrowComponent = nullptr;
};
