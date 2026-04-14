
#include "UI/Inventory/DDInventoryComponent.h"
#include "UI/Inventory/DDInventory.h"

#include "Base/Player/DDBasePlayerController.h"
#include "Common/DDLogManager.h"
#include "Data/DDItemDataTypes.h"
#include "UI/Inventory/DDInventoryBase.h"


UDDInventoryComponent::UDDInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}


void UDDInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	ConstructInventory();
	AddItem("HealingKit"); // 테스트용
}

void UDDInventoryComponent::AddItem(FName ItemName)
{
	for (TPair<FName,int32>& ItemPair : InventoryItems)
	{
		if(ItemPair.Key == ItemName)
		{
			ItemPair.Value++;
		}
	}
	
	
}

FItemTableRow* UDDInventoryComponent::GetItemData(FName RowName) const
{
	if (!ItemDataTable) return nullptr;
	return ItemDataTable->FindRow<FItemTableRow>(RowName, TEXT("Item"));
}

void UDDInventoryComponent::ConstructInventory()
{
	OwningController = Cast<ADDBasePlayerController>(GetOwner());
	checkf(OwningController.IsValid(), TEXT("플레이어의 인벤토리가 유효하지않습니다."));
	if (!OwningController->IsLocalController()) return;
	
	if (!ItemDataTable) return;
	const TArray<FName> AllItemNames = ItemDataTable->GetRowNames();
	for (const FName& ItemName : AllItemNames)
	{
		InventoryItems.Add(ItemName, 0);
	}
	
	for (const TPair<FName,int32>& ItemPair : InventoryItems)
	{
		ItemNames.Add(ItemPair.Key);
	}
	
	InventoryWidget = CreateWidget<UDDInventory>(OwningController.Get(), InventoryWidgetClass);
	InventoryWidget->AddToViewport();
	
}
