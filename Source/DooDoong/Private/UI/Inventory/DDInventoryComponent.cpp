
#include "UI/Inventory/DDInventoryComponent.h"
#include "UI/Inventory/DDInventoryWidget.h"

#include "Base/Player/DDBasePlayerController.h"
#include "Common/DDLogManager.h"
#include "Data/DDItemDataTypes.h"
#include "UI/Inventory/DDInventoryBase.h"
#include "UI/Inventory/DDItemUseButtonWidget.h"


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

void UDDInventoryComponent::ToggleInventory()
{
	if (bInventoryOpen)
	{
		CloseInventory();
	}
	else
	{
		OpenInventory();
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
	
	InventoryWidget = CreateWidget<UDDInventoryWidget>(OwningController.Get(), InventoryWidgetClass);
	InventoryWidget->AddToViewport();
	CloseInventory();
}

void UDDInventoryComponent::OpenInventory()
{
	if (!IsValid(InventoryWidget)) return;
	
	InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	InventoryWidget->RefreshGrid();
	bInventoryOpen = true;
}

void UDDInventoryComponent::CloseInventory()
{
	if (!IsValid(InventoryWidget)) return;
	
	InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	InventoryWidget->RefreshGrid();
	bInventoryOpen = false;
}
