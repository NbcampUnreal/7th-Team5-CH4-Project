// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/DDInventoryWidget.h"

#include "Base/Player/DDBasePlayerController.h"
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
	/*/* 인벤토리 컴포넌트 가져오기 #1#
	UDDInventoryComponent* DDInventoryComponent = GetOwningPlayer()->FindComponentByClass<UDDInventoryComponent>();
	if (!DDInventoryComponent) return;
	
	/* 인벤토리 컴포넌트 초기화 #1#
	InventoryComponent = DDInventoryComponent;
	if (!InventoryComponent.IsValid()) return;
	
	/* 가지고 있는 아이템 갯수 확인 #1#
	Columns = InventoryComponent->ViewItemDatas.Num();
	Rows = 1;
	GenerationGrid();
	InventoryComponent->OnInventoryChanged.AddDynamic(this, &UDDInventoryWidget::UpdateGrid);
	UpdateGrid();*/
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
			const float SlotSizeY = 200.f;
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

void UDDInventoryWidget::InitInventory(ADDBasePlayerController* Controller)
{
	if (Controller == nullptr)
	{
		//로그
		return;
	}
	
	UDDInventoryComponent* DDInventoryComponent = Controller->FindComponentByClass<UDDInventoryComponent>();
	if (DDInventoryComponent == nullptr) return;
	
	Columns = InventoryComponent->ViewItemDatas.Num();
	Rows = 1;
	GenerationGrid();
	InventoryComponent->OnInventoryChanged.AddDynamic(this, &UDDInventoryWidget::UpdateGrid);
	UpdateGrid();
}

void UDDInventoryWidget::UpdateGrid()
{
	for (int32 i = 0; i < GridSlots.Num(); ++i)
	{
		if (InventoryComponent->ViewItemDatas.IsEmpty()) return;
		GridSlots[i]->UpdateItemInfo(InventoryComponent->ViewItemDatas[i]);
	}
}


