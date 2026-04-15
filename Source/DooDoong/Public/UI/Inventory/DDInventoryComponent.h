
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DDInventoryComponent.generated.h"

class UDDItemUseButtonWidget;
struct FItemTableRow;
class UDataTable;
class UDDInventoryWidget;
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
	void AddItem(FName ItemName);
	
	void ToggleInventory();
	
	void UseItem(const FName& ItemSlotName);
	
	UPROPERTY(VisibleAnywhere, Category= "Inventory|Items")
	TMap<FName, int32> InventoryItems;
	
	UPROPERTY(VisibleAnywhere, Category= "Inventory|Items")
	TArray<FName> ItemNames;
	
	FItemTableRow* GetItemData(FName RowName) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category= "Inventory|Data")
	TObjectPtr<UDataTable> ItemDataTable;
	
private:
	TWeakObjectPtr<ADDBasePlayerController> OwningController;
	
	void ConstructInventory();
	
	UPROPERTY()
	TObjectPtr<UDDInventoryWidget> InventoryWidget;
	
	UPROPERTY(EditDefaultsOnly, Category= "Inventory")
	TSubclassOf<UDDInventoryWidget> InventoryWidgetClass;
	
	bool bInventoryOpen;
	void OpenInventory();
	void CloseInventory();
	
};
