
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DDInventoryComponent.generated.h"

struct FItemTableRow;
class UDataTable;
class UDDInventory;
class ADDBasePlayerController;
class UDDInventoryBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class DOODOONG_API UDDInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDDInventoryComponent();

public:
	virtual void BeginPlay() override;
	
public:
	void AddItem();
	
	const FItemTableRow* GetItemData(FName RowName) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category= "Inventory|Data")
	TObjectPtr<UDataTable> ItemDataTable;
	
private:
	TWeakObjectPtr<ADDBasePlayerController> OwningController;
	
	void ConstructInventory();
	
	UPROPERTY()
	TObjectPtr<UDDInventory> InventoryWidget;
	
	UPROPERTY(EditDefaultsOnly, Category= "Inventory")
	TSubclassOf<UDDInventory> InventoryWidgetClass;
	
};
