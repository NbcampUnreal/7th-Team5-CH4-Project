#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "DDMoveTileStep.generated.h"

class ADDTile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveFinished);
UCLASS()
class DOODOONG_API UDDMoveTileStep : public UAbilityTask
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintAssignable)
	FOnMoveFinished OnFinished;

	// Dump: 임시 현재 타일 // 나중에 지울 것
	// UPROPERTY(EditAnywhere)
	// ADDTile* CurrentTile;
	
	static UDDMoveTileStep* MoveTile(
		UGameplayAbility* OwningAbility,
		int32 Steps
	);

	virtual void Activate() override;

private:

	int32 RemainingSteps;

	void MoveNext();
};
