// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/DDInventory.h"
#include "UI/Inventory/DDInvenGridSlot.h"
#include "Common/DDLogManager.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UDDInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ConstructGrid();
}

void UDDInventory::ConstructGrid()
{
	GridSlots.Reserve(Rows * Columns);
	
	for (int32 j = 0; j < Rows; ++j)
	{
		for (int32 i = 0; i < Columns; ++i)
		{
			LOG_PMJ(Warning, TEXT("Create GridSlot"));
			UDDInvenGridSlot* GridSlot = CreateWidget<UDDInvenGridSlot>(this, GridSlotClass);
			UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CanvasPanel->AddChild(GridSlot));
			
			const float SlotSize = 128.f;
			const float Spacing = 24.f;

			
			int32 Index = i + j * Columns;
			if (CanvasSlot)
			{
				CanvasSlot->SetSize(FVector2D(SlotSize, SlotSize));
				CanvasSlot->SetPosition(FVector2D(
					i * (SlotSize + Spacing),
					j * (SlotSize + Spacing)
					));
			}
		}
	}
}
