#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemActionComponent.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum class EItemActionMode : uint8
{
	None,
	Instant,
	Targeting,
	Range
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DOODOONG_API UItemActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemActionComponent();
	
public:
	void BeginItemAction(const FName ItemID);
	
	void TryActivateItem();
	
	void SelectNextTarget();
	
	void SelectPreviousTarget();
	
	void ConfirmItemAction();
	
	void CancelItemAction();
	
protected:
	UPROPERTY()
	TObjectPtr<UDataTable> ItemData;	
	
	UPROPERTY()
	EItemActionMode CurrentActionMode = EItemActionMode::None;

	UPROPERTY()
	FName ActiveItemID = NAME_None;

	UPROPERTY()
	TSubclassOf<UGameplayAbility> ActiveItemAbility;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> CandidateTargets;

	UPROPERTY()
	int32 SelectedTargetIndex = INDEX_NONE;
	
};
