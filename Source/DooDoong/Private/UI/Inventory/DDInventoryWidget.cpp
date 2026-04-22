
#include "UI/Inventory/DDInventoryWidget.h"

#include "AbilitySystemComponent.h"
#include "Base/Player/DDBasePlayerController.h"
#include "BoardGame/Game/DDBoardGameState.h"
#include "UI/Inventory/DDInvenGridSlot.h"
#include "UI/Inventory/DDInventoryComponent.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Common/DDLogManager.h"
#include "MiniGames/Platformer/GameState/DDPlatformerGameState.h"

void UDDInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UDDInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	LOG_KMS(Warning, TEXT("[NativeConstruct] ")); 
	ADDBoardGameState* DDGS = GetWorld()->GetGameState<ADDBoardGameState>();
	if (!DDGS)
	{
		LOG_KMS(Warning, TEXT("[NativeConstruct] : GameState is Null.")); 
		return;
	}
	
	for (const auto& PlayerState : DDGS->PlayerArray)
	{
		
		ADDBasePlayerState* DDPS = Cast<ADDBasePlayerState>(PlayerState);
		if (DDPS == nullptr)
		{
			LOG_KMS(Warning, TEXT("[NativeConstruct] : PlayerState is Null.")); 
			continue;
		}
		
		UAbilitySystemComponent* ASC = DDPS->GetAbilitySystemComponent();
		if (ASC == nullptr)
		{
			LOG_KMS(Warning, TEXT("[NativeConstruct] : ASC is Null.")); 
			continue; 
		}
		
		if (ASC->HasMatchingGameplayTag(DDGameplayTags::State_BoardGame_TurnActive))
		{
			LOG_KMS(Warning, TEXT("Matching Tag Found.")); 
			InitInventory(DDPS->GetInventoryComponent());
			break;
		}
	}
}

void UDDInventoryWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	if (InventoryComponent)
		InventoryComponent->OnInventoryChanged.RemoveAll(this);
}

void UDDInventoryWidget::GenerateGrid()
{
	LOG_KMS(Warning, TEXT("GenerateGrid"));
	
	if (!InventoryComponent)
	{
		LOG_KMS(Warning, TEXT("[GenerationGrid] : Inventory is null."));
		return;
	}
	LOG_PMJ(Warning, TEXT("Columns : %d"), InventoryComponent->ViewItemDatas.Num());
	
	for (int32 j = 0; j < Rows; ++j)
	{
		for (int32 i = 0; i < Columns; ++i)
		{
			if (InventoryComponent->ViewItemDatas.IsEmpty())
			{
				LOG_PMJ(Error, TEXT("===== UI출력용 아이템 데이터가 존재하지 않습니다! ====="));
				continue;
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
			
			LOG_PMJ(Warning, TEXT("InventoryComponent->ViewItemDatas[Index].Name : %s"), *InventoryComponent->ViewItemDatas[Index].ViewItemName.ToString());
			GridSlot->SetItemInfo(InventoryComponent->ViewItemDatas[Index]);
			GridSlots.Add(GridSlot);
		}
	}
}

void UDDInventoryWidget::InitInventory(UDDInventoryComponent* Inventory)
{
	LOG_KMS(Warning, TEXT("인벤토리 초기화"));
	if (!Inventory) return; 
	
	InventoryComponent = Inventory; 
	
	Columns = InventoryComponent->ViewItemDatas.Num();
	Rows = 1;
	GenerateGrid();
	
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


