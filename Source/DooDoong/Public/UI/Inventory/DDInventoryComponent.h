
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DDInventoryComponent.generated.h"

class ADDBasePlayerController;
class UDDInventoryBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class DOODOONG_API UDDInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDDInventoryComponent();

protected:
	virtual void BeginPlay() override;
	
private:
	TWeakObjectPtr<ADDBasePlayerController> OwningController;
	
	void ConstructInventory();
	
	UPROPERTY()
	TObjectPtr<UDDInventoryBase> InventoryMenu;
	
	UPROPERTY(EditDefaultsOnly, Category= "Inventory")
	TSubclassOf<UDDInventoryBase> InventoryMenuClass;
	
	
};
