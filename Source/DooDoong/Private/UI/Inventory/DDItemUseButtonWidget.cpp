#include "UI/Inventory/DDItemUseButtonWidget.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Base/Player/DDBasePlayerController.h"
#include "Common/DDLogManager.h"
#include "UI/Inventory/DDInventoryComponent.h"
#include "UI/Inventory/DDInventoryWidget.h"
#include "Components/Button.h"
#include "Data/DDItemDataTypes.h"
#include "System/DDGameInstance.h"
#include "UI/Inventory/DDInvenGridSlot.h"

void UDDItemUseButtonWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
}

void UDDItemUseButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BT_Use)
	{
		BT_Use->OnClicked.AddDynamic(this, &UDDItemUseButtonWidget::UseButton);
	}
		
	if (BT_Cancel)
	{
		BT_Cancel->OnClicked.AddDynamic(this, &UDDItemUseButtonWidget::CancelButton);
	}
}

void UDDItemUseButtonWidget::UseButton()
{
	ADDBasePlayerController* PC = Cast<ADDBasePlayerController>(GetOwningPlayer());
	if (PC == nullptr) return;
	ADDBasePlayerState* PS = PC->GetPlayerState<ADDBasePlayerState>();
	if (PS == nullptr) return;
	UDDInventoryComponent* InventoryComponent = Cast<UDDInventoryComponent>(PS->GetInventoryComponent());
	if (InventoryComponent == nullptr)
	{
		LOG_PMJ(Error, TEXT(" UseButtonWidget : 인벤토리 컴포넌트 없어요"));
		return;
	}
	InventoryComponent->Server_UseItem(CurrentItemSlotName);
	this->SetVisibility(ESlateVisibility::Collapsed);
}

void UDDItemUseButtonWidget::CancelButton()
{
	
	if (OwningGridSlot != nullptr)
    {
        OwningGridSlot->BT_ClickItem->SetIsEnabled(true);
    }
	this->SetVisibility(ESlateVisibility::Collapsed);
}

void UDDItemUseButtonWidget::InitializeGridSlotData(const FName& SlotItemName, UDDInvenGridSlot* InParentSlot)
{
	if (SlotItemName.IsNone()) return;
	CurrentItemSlotName = SlotItemName;
	
	if (CurrentItemSlotName.IsNone()) return;
	
	FItemTableRow* ItemTableRow = UDDGameInstance::Get(GetWorld())->ItemDataTable->FindRow<FItemTableRow>(CurrentItemSlotName, TEXT("GetItemData"));
	if (ItemTableRow == nullptr) return;
	
	if (ET_ItemDescription)
	{
		ET_ItemDescription->SetText(FText(ItemTableRow->Description));
	}
	
	if (ET_ItemName)
	{
		ET_ItemName->SetText(FText(ItemTableRow->DisplayName));
	}
}
