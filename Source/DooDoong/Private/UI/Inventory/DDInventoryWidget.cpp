// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/DDInventoryWidget.h"
#include "UI/Inventory/DDInvenGridSlot.h"
#include "UI/Inventory/DDInventoryComponent.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Common/DDLogManager.h"
#include "Data/DDItemDataTypes.h"

void UDDInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	
}

void UDDInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UDDInventoryComponent* DDInventoryComponent = GetOwningPlayer()->FindComponentByClass<UDDInventoryComponent>();
	if (!DDInventoryComponent) return;
	InventoryComponent = DDInventoryComponent;
	if (!InventoryComponent.IsValid()) return;
	Columns = InventoryComponent->ViewItemDatas.Num();
	Rows = 1;
	GenerationGrid();
	InventoryComponent->OnInventoryChanged.AddDynamic(this, &UDDInventoryWidget::UpdateGrid);
	UpdateGrid();
}

void UDDInventoryWidget::NativeDestruct()
{
	Super::NativeDestruct();
	InventoryComponent->OnInventoryChanged.RemoveAll(this);
}

void UDDInventoryWidget::GenerationGrid()
{
	for (int32 j = 0; j < Rows; ++j)
	{
		for (int32 i = 0; i < Columns; ++i)
		{
			if (InventoryComponent->ViewItemDatas.IsEmpty())
			{
				LOG_PMJ(Error, TEXT("===== UI출력용 아이템 데이터가 존재하지 않습니다! ====="));
				return;
			}
			int32 Index = i + j * Columns;
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
			GridSlot->SetItemInfo(InventoryComponent->ViewItemDatas[Index]);
			GridSlots.Add(GridSlot);
		}
	}
}

void UDDInventoryWidget::UpdateGrid()
{
	for (int32 i = 0; i < GridSlots.Num(); ++i)
	{
		if (InventoryComponent->ViewItemDatas.IsEmpty()) return;
		GridSlots[i]->UpdateItemInfo(InventoryComponent->ViewItemDatas[i]);
	}
}


