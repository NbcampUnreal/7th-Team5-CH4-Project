#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/DDItemDataTypes.h"
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
	void BeginItemAction(FName ItemID, const FItemTableRow& ItemRow);
	
	void TryActivateItem();
	
	void ConfirmItemAction();
	
	void CancelItemAction();
	
protected:
	void BuildTargetCandidates();
	void SelectNextTarget();
	void SelectPreviousTarget();
	
protected:
	UPROPERTY()
	TObjectPtr<UDataTable> ItemData;	
	
	UPROPERTY()
	EItemActionMode CurrentActionMode = EItemActionMode::None;
	
	UPROPERTY()
	FName ActiveItemID = NAME_None;
	
	UPROPERTY()
	FGameplayTag ActiveItemType;
	
	UPROPERTY()
	TSubclassOf<UGameplayAbility> ActiveItemAbility;
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> CandidateTargets;
	
	UPROPERTY()
	int32 SelectedTargetIndex = INDEX_NONE;
	
};
