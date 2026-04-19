// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/DDItemUseButtonWidget.h"
#include "UI/Inventory/DDInventoryComponent.h"
#include "UI/Inventory/DDInventoryWidget.h"
#include "Components/Button.h"
#include "UI/Inventory/DDInvenGridSlot.h"

void UDDItemUseButtonWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	InventoryComponent = GetOwningLocalPlayer()->GetPlayerController(GetWorld())->FindComponentByClass<UDDInventoryComponent>();
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
	if (GetOwningLocalPlayer())
	{
		InventoryComponent->Server_UseItem(CurrentItemSlotName);
		this->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDDItemUseButtonWidget::CancelButton()
{
	if (GetOwningLocalPlayer())
	{
		UDDInvenGridSlot* ParentGridSlot = Cast<UDDInvenGridSlot>(GetParent());
		if (ParentGridSlot != nullptr)
		{
			ParentGridSlot->BT_ClickItem->SetIsEnabled(true);
		}
		this->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDDItemUseButtonWidget::InitializeGridSlotData(const FName& SlotItemName)
{
	if (SlotItemName.IsNone()) return;
	CurrentItemSlotName = SlotItemName;
}
