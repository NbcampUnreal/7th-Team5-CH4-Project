#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "DDMoveTileStepTask.generated.h"

class ADDTile;
class ADDSelectableTileActor;

UCLASS()
class DOODOONG_API UDDMoveTileStepTask : public UAbilityTask
{
	GENERATED_BODY()

public:
	static UDDMoveTileStepTask* MoveTile(
		UGameplayAbility* OwningAbility,
		int32 Steps
	);

	virtual void Activate() override;

	void MoveNext();

	void ContinueMove(ADDTile* Tile);

	UFUNCTION()
	void OnCharacterMoveFinished();
	void OnDestroy(bool AbilityEnded);

	void SelectNextTile(ADDTile* SelectedTile);
	
	UPROPERTY(BlueprintAssignable)
	FOnMoveFinished OnFinished;

private:
	int32 RemainingSteps = 0;

	UPROPERTY()
	TArray<ADDSelectableTileActor*> SpawnedActors;

	UPROPERTY()
	ADDTile* PendingTile; // 선택 대기용

	void SpawnSelectableActors(const TArray<ADDTile*>& Tiles);

	void ClearSelectableActors();

public:
};
