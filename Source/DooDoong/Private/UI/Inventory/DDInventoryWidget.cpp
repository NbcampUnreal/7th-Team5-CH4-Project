// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/DDInventoryWidget.h"

#include "Base/Player/DDBasePlayerController.h"
#include "UI/Inventory/DDInvenGridSlot.h"
#include "Common/DDLogManager.h"
#include "UI/Inventory/DDInventoryComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Data/DDItemDataTypes.h"

void UDDInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(GetOwningPlayer());
	if (IsValid(DDPC) == true)
	{
		InventoryComponent = DDPC->GetComponentByClass<UDDInventoryComponent>();
	}
	Columns = InventoryComponent->InventoryItems.Num();
	Rows = 1;
	GenerationGrid();	
}

void UDDInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshGrid();
}

void UDDInventoryWidget::GenerationGrid()
{
	GridSlots.Reserve(Rows * Columns);
	
	for (int32 j = 0; j < Rows; ++j)
	{
		for (int32 i = 0; i < Columns; ++i)
		{
			UDDInvenGridSlot* GridSlot = CreateWidget<UDDInvenGridSlot>(this, GridSlotClass);
			UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CanvasPanel->AddChild(GridSlot));
			
			const float SlotSizeX = 200.f;
			const float SlotSizeY = 150.f;
			const float SpacingX = 100.f;
			
			if (CanvasSlot)
			{
				CanvasSlot->SetSize(FVector2D(SlotSizeX, SlotSizeY));
				CanvasSlot->SetPosition(FVector2D(
					i * (SlotSizeX + SpacingX) + 50.f,
					j * (SlotSizeX) + 50.f
					));
			}
			int32 Index = i + j * Columns;
			if (!InventoryComponent->ItemNames.IsValidIndex(Index)) return;
			FName ItemName = InventoryComponent->ItemNames[Index];
			FItemTableRow* ItemData = InventoryComponent->GetItemData(ItemName);
			GridSlot->SetItemInfo(ItemData);
			GridSlots.Add(GridSlot);
		}
	}
}

void UDDInventoryWidget::RefreshGrid()
{
	LOG_PMJ(Error, TEXT("Call RefreshGrid"));
	for (const auto& GridSlot : GridSlots)
	{
		GridSlot->UpdateItemInfo(InventoryComponent->InventoryItems);
	}
}

