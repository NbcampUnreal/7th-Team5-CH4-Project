
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

	if (ItemDataTable)
	{
		LOG_PMJ(Warning, TEXT("ItemDataTable %s"), *ItemDataTable->GetName());
	}
	
	if (const FItemTableRow* Row = GetItemData(TEXT("HealingKit")))
	{
		LOG_PMJ(Warning, TEXT("%s"), *Row->DisplayName.ToString());
	}
	
	ConstructInventory();	
}

const FItemTableRow* UDDInventoryComponent::GetItemData(FName RowName) const
{
	if (!ItemDataTable) return nullptr;
	return ItemDataTable->FindRow<FItemTableRow>(RowName, TEXT("Item"));
}

void UDDInventoryComponent::ConstructInventory()
{
	OwningController = Cast<ADDBasePlayerController>(GetOwner());
	checkf(OwningController.IsValid(), TEXT("플레이어의 인벤토리가 유효하지않습니다."));
	if (!OwningController->IsLocalController()) return;
	
	InventoryWidget = CreateWidget<UDDInventory>(OwningController.Get(), InventoryWidgetClass);
	InventoryWidget->AddToViewport();
	
}
