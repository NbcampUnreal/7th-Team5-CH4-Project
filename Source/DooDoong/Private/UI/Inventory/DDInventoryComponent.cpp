
#include "UI/Inventory/DDInventoryComponent.h"
#include "UI/Inventory/DDInventoryWidget.h"
#include "UI/Inventory/DDInventoryBase.h"

#include "Base/Character/DDBaseCharacter.h"
#include "Base/Player/DDBasePlayerController.h"

#include "BoardGame/Character/Components/ItemActionComponent.h"
#include "Common/DDLogManager.h"
#include "Data/DDItemDataTypes.h"
#include "Net/UnrealNetwork.h"
#include "UI/Inventory/DDItemUseButtonWidget.h"


UDDInventoryComponent::UDDInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


void UDDInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
	{
		InitializeInventoryData();
		/* 테스트용 아이템추가 */
		ServerRPCAddItem("HealingKit"); 
		ServerRPCAddItem("GiveBomb");
	}
	
	InitializeInventoryUI();
	
}

void UDDInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, InventoryItemDatas);
}

void UDDInventoryComponent::InitializeInventoryData()
{
	if (!ItemDataTable) return;
	const TArray<FName> AllItemNames = ItemDataTable->GetRowNames();
	for (int32 i = 0; i < AllItemNames.Num(); ++i)
	{
		FInventoryItemData InventoryItemData;
		InventoryItemData.ItemName = AllItemNames[i];
		InventoryItemData.Count = 0;
		InventoryItemDatas.Add(InventoryItemData);
	}
}

void UDDInventoryComponent::InitializeInventoryUI()
{
	OwningController = Cast<ADDBasePlayerController>(GetOwner());
	if (OwningController == nullptr) return;
	if (OwningController->IsLocalController())
	{
		OwningController->Client_CreateInventoryUI();
	}
}

FItemTableRow* UDDInventoryComponent::GetItemData(FName RowName) const
{
	if (!ItemDataTable) return nullptr;
	return ItemDataTable->FindRow<FItemTableRow>(RowName, TEXT("Item"));
}

void UDDInventoryComponent::OnRep_ClientInventoryData()
{
	LOG_PMJ(Warning, TEXT("OnRep_ClientInventoryData"));
}



void UDDInventoryComponent::ServerRPCAddItem_Implementation(FName ItemName)
{
	if (!(GetOwner()->HasAuthority())) return;
	LOG_PMJ(Warning, TEXT("ServerRPCAddItem 진입"));
	if (InventoryItemDatas.IsEmpty())
	{
		LOG_PMJ(Warning, TEXT("인벤데이터가 비어있습니다"));
		return;
	}
	for (FInventoryItemData& InventoryItemData : InventoryItemDatas)
	{
		if (InventoryItemData.ItemName == ItemName)
		{
			LOG_PMJ(Error, TEXT("====== 아이템 갯수 + 1 ======"));
			InventoryItemData.Count++;
		}
	}
}

void UDDInventoryComponent::ServerRPCUseItem_Implementation(const FName& ItemSlotName)
{
	if (ItemSlotName.IsNone()) return;
	for (FInventoryItemData& ItemDatas : InventoryItemDatas)
	{
		if (ItemDatas.ItemName == ItemSlotName)
		{
			UItemActionComponent* ISC = OwningController->GetPawn()->FindComponentByClass<UItemActionComponent>();
			if (!IsValid(ISC)) return;
			FItemTableRow& CurrentItemDataRow = *GetItemData(ItemDatas.ItemName);
			ISC->BeginItemAction(CurrentItemDataRow);
			ItemDatas.Count--;
		}
	}
}
