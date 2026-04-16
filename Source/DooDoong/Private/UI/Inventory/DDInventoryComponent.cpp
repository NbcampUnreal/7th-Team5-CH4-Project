
#include "UI/Inventory/DDInventoryComponent.h"
#include "UI/Inventory/DDInventoryWidget.h"
#include "UI/Inventory/DDInventoryBase.h"

#include "Base/Character/DDBaseCharacter.h"
#include "Base/Player/DDBasePlayerController.h"

#include "BoardGame/Character/Components/ItemActionComponent.h"
#include "Common/DDLogManager.h"
#include "Data/DDItemDataTypes.h"
#include "UI/Inventory/DDItemUseButtonWidget.h"


UDDInventoryComponent::UDDInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


void UDDInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	ConstructInventory();
	
	/* 테스트용 아이템추가 */
	AddItem("HealingKit"); 
	AddItem("GiveBomb");
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

void UDDInventoryComponent::RequestOpenInventory()
{
	OpenInventory();
}

void UDDInventoryComponent::RequestCloseInventory()
{
	CloseInventory();
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

void UDDInventoryComponent::UseItem(const FName& ItemSlotName)
{
	if (ItemSlotName.IsNone()) return;
	for (TPair<FName,int32>& ItemPair : InventoryItems)
	{
		if (ItemPair.Key == ItemSlotName)
		{
			UItemActionComponent* ISC = OwningController->GetPawn()->FindComponentByClass<UItemActionComponent>();
			if (!IsValid(ISC)) return;
			FItemTableRow& CurrentItemDataRow = *GetItemData(ItemPair.Key);
			CloseInventory(); // 아이템 사용 시 인벤토리 창을 닫음.
			ISC->BeginItemAction(CurrentItemDataRow);
			ItemPair.Value--;
			InventoryWidget->RefreshGrid();
		}
	}
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
