// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/DDInvenGridSlot.h"
#include "UI/Inventory/DDItemUseButtonWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UDDInvenGridSlot::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BT_UseItem)
	{
		BT_UseItem->OnClicked.AddDynamic(this, &UDDInvenGridSlot::UseItem);
		BT_UseItem->SetIsEnabled(bCanUse);
	}
}

void UDDInvenGridSlot::SetItemInfo(UTexture2D* Image) const
{
	Image_ItemIcon->SetBrushFromTexture(Image);
}

void UDDInvenGridSlot::UseItem()
{
	ItemUseButtonWidget = CreateWidget<UDDItemUseButtonWidget>(this, ItemUseButtonWidgetClass);
	if (ItemUseButtonWidget)
	{
		ItemUseButtonWidget->AddToViewport();
	}
}
