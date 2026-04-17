// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/DDInventoryWidget.h"
#include "UI/Inventory/DDInvenGridSlot.h"
#include "UI/Inventory/DDInventoryComponent.h"
#include "Base/Player/DDBasePlayerController.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Data/DDItemDataTypes.h"
#include "Common/DDLogManager.h"

void UDDInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	InventoryComponent = GetOwningPlayer()->GetComponentByClass<UDDInventoryComponent>();
	if (!InventoryComponent.IsValid()) return;
	Columns = InventoryComponent->InventoryItemDatas.Num();
	Rows = 1;
	if (GetOwningPlayer()->IsLocalController())
	{
		GenerationGrid();	
		BindItemData();
	}
}

void UDDInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDDInventoryWidget::GenerationGrid()
{
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
			GridSlots.Add(GridSlot);
		}
	}
}

void UDDInventoryWidget::Client_RefreshGrid_Implementation()
{
	LOG_PMJ(Error, TEXT("GridSlot.Num : %d"), GridSlots.Num());
	if (!InventoryComponent.IsValid() || !GetOwningLocalPlayer()) return;
	for (int32 i = 0; i < GridSlots.Num(); ++i)
	{
		GridSlots[i]->UpdateItemInfo(InventoryComponent->InventoryItemDatas[i]);
	}
}

void UDDInventoryWidget::BindItemData()
{
	if (GridSlots.Num() <= 0) return;
	for (int32 i =0; i < GridSlots.Num(); ++i)
	{
		if (!InventoryComponent->InventoryItemDatas.IsValidIndex(i)) return;
		FName ItemName = InventoryComponent->InventoryItemDatas[i].ItemName;
		FItemTableRow& ItemData = *InventoryComponent->GetItemData(ItemName);
		GridSlots[i]->SetItemInfo(ItemData);
	}
	
}

