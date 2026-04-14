// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/DDInvenGridSlot.h"
#include "UI/Inventory/DDItemUseButtonWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Data/DDItemDataTypes.h"

void UDDInvenGridSlot::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ItemCount != 0)
	{
		bCanUse = true;
	}
	
	if (BT_ClickItem)
	{
		BT_ClickItem->OnClicked.AddDynamic(this, &UDDInvenGridSlot::UseItem);
		BT_ClickItem->SetIsEnabled(bCanUse);
	}
}

void UDDInvenGridSlot::SetItemInfo(FItemTableRow* ItemTableRow)
{
	// ItemTable 가져와서 처음 아이콘 이랑 아이템ID 설정
	if (ItemTableRow == nullptr) return;
	UTexture2D* ItemIcon = ItemTableRow->Icon.LoadSynchronous();
	Image_ItemIcon->SetBrushFromTexture(ItemIcon);
	ItemName = ItemTableRow->ItemID;
}

void UDDInvenGridSlot::UpdateItemInfo(const TMap<FName, int32>& InventoryItemData)
{
	// 인벤토리 열릴때마다 인벤토리내부 데이터 전달받아서 아이템이름확인하고 갯수맞춰주기
	for (const TPair<FName, int32>& ItemPair : InventoryItemData)
	{
		if (ItemName == ItemPair.Key)
		{
			ItemCount = ItemPair.Value;
		}
	}
}

void UDDInvenGridSlot::UseItem()
{
	ItemUseButtonWidget = CreateWidget<UDDItemUseButtonWidget>(this, ItemUseButtonWidgetClass);
	if (ItemUseButtonWidget)
	{
		ItemUseButtonWidget->AddToViewport();
	}
}
