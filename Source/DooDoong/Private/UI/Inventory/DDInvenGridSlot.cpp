// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/DDInvenGridSlot.h"

#include "Common/DDLogManager.h"
#include "UI/Inventory/DDItemUseButtonWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Data/DDItemDataTypes.h"
#include "UI/Inventory/DDInventoryComponent.h"

void UDDInvenGridSlot::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BT_ClickItem)
	{
		BT_ClickItem->OnClicked.AddDynamic(this, &UDDInvenGridSlot::UseItem);
	}
}

void UDDInvenGridSlot::SetItemInfo(const FItemTableRow& ItemTableRow)
{
	// ItemTable 가져와서 처음 아이콘 이랑 아이템ID 설정
	UTexture2D* ItemIcon = ItemTableRow.Icon.LoadSynchronous();
	Image_ItemIcon->SetBrushFromTexture(ItemIcon);
	ItemName = ItemTableRow.ItemID;
}

void UDDInvenGridSlot::UpdateItemInfo(const FInventoryItemData& InventoryItemData)
{
	// 인벤토리 열릴때마다 인벤토리내부 데이터 전달받아서 아이템이름확인하고 갯수맞춰주기
	if (!InventoryItemData.ItemName.IsValid()) return;
	ItemCount = InventoryItemData.Count;
	
	LOG_PMJ(Warning, TEXT("ItemCount : %d"), ItemCount);
	if (ItemCount > 0)
	{
		bCanUse = true;
	}
	else
	{
		bCanUse = false;
	}
	BT_ClickItem->SetIsEnabled(bCanUse);
}

void UDDInvenGridSlot::UseItem()
{
	ItemUseButtonWidget = CreateWidget<UDDItemUseButtonWidget>(this, ItemUseButtonWidgetClass);
	if (ItemUseButtonWidget)
	{
		ItemUseButtonWidget->AddToViewport();
		ItemUseButtonWidget->InitializeGridSlotData(ItemName);
		BT_ClickItem->SetIsEnabled(false);
	}
}
