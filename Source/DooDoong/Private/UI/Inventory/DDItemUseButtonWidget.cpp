// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/DDItemUseButtonWidget.h"
#include "UI/Inventory/DDInventoryComponent.h"
#include "UI/Inventory/DDInventoryWidget.h"

#include "Base/Player/DDBasePlayerController.h"
#include "Components/Button.h"
#include "Data/DDItemDataTypes.h"

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
		InventoryComponent->ServerRPCUseItem(CurrentItemSlotName);
		this->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDDItemUseButtonWidget::CancelButton()
{
	if (GetOwningLocalPlayer())
	{
		this->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDDItemUseButtonWidget::InitializeGridSlotData(const FName& SlotItemName)
{
	if (SlotItemName.IsNone()) return;
	CurrentItemSlotName = SlotItemName;
}
