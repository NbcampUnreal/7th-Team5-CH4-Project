// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/DDInvenGridSlot.h"

#include <string>

#include "Common/DDLogManager.h"
#include "UI/Inventory/DDItemUseButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Data/DDItemDataTypes.h"
#include "UI/Inventory/DDInventoryComponent.h"
#include "UI/Inventory/DDInventoryWidget.h"

void UDDInvenGridSlot::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BT_ClickItem)
	{
		BT_ClickItem->OnClicked.AddDynamic(this, &UDDInvenGridSlot::UseItem);
	}
}

void UDDInvenGridSlot::SetItemInfo(const FViewItemData& ViewItemData)
{
	// ItemTable 가져와서 처음 아이콘 이랑 아이템ID 설정
	if (ViewItemData.Icon.IsNull() || ViewItemData.ViewItemName.IsNone())
	{
		LOG_PMJ(Error, TEXT("===== 아이템의 정보를 참조할 수 없습니다 존재하지않습니다! ====="));
		return;
	}
	UTexture2D* ItemIcon = ViewItemData.Icon.LoadSynchronous();
	Image_ItemIcon->SetBrushFromTexture(ItemIcon);
	ItemName = ViewItemData.ViewItemName;
	ItemCount = ViewItemData.ViewItemCount;
	BT_ClickItem->SetIsEnabled(false);
}

void UDDInvenGridSlot::UpdateItemInfo(const FViewItemData& ViewItemData)
{
	// 인벤토리 열릴때마다 인벤토리내부 데이터 전달받아서 아이템이름확인하고 갯수맞춰주기
	if (!ViewItemData.ViewItemName.IsValid()) return;
	
	if (!TextBlock_ItemCount) return;
	
	if (ViewItemData.ViewItemName == ItemName)
	{
		ItemCount = ViewItemData.ViewItemCount;
	
		if (ItemCount > 0)
		{
			TextBlock_ItemCount->SetText(FText::FromString(FString::FromInt(ItemCount)));
			bCanUse = true;
		}
		else
		{
			bCanUse = false;
		}
		BT_ClickItem->SetIsEnabled(bCanUse);
	}
}

void UDDInvenGridSlot::UseItem()
{
	ItemUseButtonWidget = CreateWidget<UDDItemUseButtonWidget>(this, ItemUseButtonWidgetClass);
	if (ItemUseButtonWidget)
	{
		ItemUseButtonWidget->AddToViewport(30);
		ItemUseButtonWidget->InitializeGridSlotData(ItemName);
		BT_ClickItem->SetIsEnabled(false);
	}
}
