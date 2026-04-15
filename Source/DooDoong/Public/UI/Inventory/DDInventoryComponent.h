
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

USTRUCT(BlueprintType)
struct FInventoryItemReplicator
{
	GENERATED_BODY()
	
	UPROPERTY()
	FName ItemName;
	UPROPERTY()
	int32 Count;
	
};

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
	
	
	/* 레플리케이티드가 된다면 서버에서 획득삭제시 클라에 동기화 */
	/* 서버전용 인벤토리 변수 */
	UPROPERTY(VisibleAnywhere, Category= "Inventory|Items")
	TMap<FName, int32> InventoryItems;
	
	UPROPERTY(VisibleAnywhere, Category= "Inventory|Items")
	TArray<FName> ItemNames;
	
	FItemTableRow* GetItemData(FName RowName) const;
	

	/* 클라이언트 전용 복제 변수 */
	UPROPERTY(ReplicatedUsing=OnRep_ClientInventoryData)
	TArray<FInventoryItemReplicator> ClientInventoryData;
	
	UFUNCTION()
	void OnRep_ClientInventoryData();	
	
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
