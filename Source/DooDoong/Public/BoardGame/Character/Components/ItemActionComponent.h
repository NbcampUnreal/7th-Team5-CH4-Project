#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemActionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DOODOONG_API UItemActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemActionComponent();
	
public:
	void BeginItemAction(const FName ItemID);
	
	void TryActivateItem();
	
	void SelectNextTarget(AActor* Target);
	
	void SelectPreviousTarget(AActor* Target);
	
	void ConfirmItemAction();
	
	void CancelItemAction();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemActionComp")
	TObjectPtr<UDataTable> ItemData;
	
};
