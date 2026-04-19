
#include "UI/Inventory/DDInventoryComponent.h"
#include "Base/Character/DDBaseCharacter.h"
#include "Base/Player/DDBasePlayerController.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "BoardGame/Character/Components/ItemActionComponent.h"
#include "Common/DDLogManager.h"
#include "Data/DDItemDataTypes.h"
#include "Net/UnrealNetwork.h"



UDDInventoryComponent::UDDInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	OwningController = Cast<ADDBasePlayerController>(GetOwner());
	
}


void UDDInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	/*if (GetOwner()->HasAuthority())
	{
		InitializeInventoryData();
		/* 테스트용 아이템추가 #1#
		Server_AddItem("HealingKit"); 
	}*/
		/* 테스트용 아이템추가 
		ServerRPCAddItem("HealingKit"); 
		ServerRPCAddItem("GiveBomb");
		ServerRPCAddItem("Magnet");
		ServerRPCAddItem("MeleeDamage");
	}*/
}

void UDDInventoryComponent::Server_InitializeInventoryData_Implementation()
{
	if (!(GetOwner()->HasAuthority())) return;
	if (!ItemDataTable) return;
	const TArray<FName>& AllItemNames = ItemDataTable->GetRowNames();
	if (AllItemNames.IsEmpty()) return;
	for (int32 i = 0; i < AllItemNames.Num(); ++i)
	{
		FItemTableRow* ItemDataRow = ItemDataTable->FindRow<FItemTableRow>(AllItemNames[i], TEXT("FindItem"));
		if (!ItemDataRow) return;
		FInventoryItemData InventoryItemData;
		InventoryItemData.ItemName = AllItemNames[i];
		InventoryItemData.ItemCount = 0;
		InventoryItemData.bCanUse = false;
		InventoryItemData.Icon = ItemDataRow->Icon;
		InventoryItemDatas.Add(InventoryItemData);
		
		FViewItemData ViewItemData;
		ViewItemData.ViewItemName = AllItemNames[i];
		ViewItemData.ViewItemCount = 0;
		ViewItemData.bCanUse = false;
		ViewItemData.Icon = ItemDataRow->Icon;
		ViewItemDatas.Add(ViewItemData);
	}
}


void UDDInventoryComponent::RequestOpenInventory()
{
	OwningController->Client_OpenInventory();
}

void UDDInventoryComponent::RequestCloseInventory()
{
	OwningController->Client_CloseInventory();
}

void UDDInventoryComponent::RefreshInventory()
{
	if (InventoryItemDatas.IsEmpty())
	{
		LOG_PMJ(Error, TEXT("===== 원본 인벤토리 데이터가 비어있습니다 ====="));
		return;
	}
	for (int32 i = 0; i < InventoryItemDatas.Num(); ++i)
	{
		//ViewItemDatas[i].ViewItemName = InventoryItemDatas[i].ItemName;
		ViewItemDatas[i].ViewItemCount = InventoryItemDatas[i].ItemCount;
		//ViewItemDatas[i].bCanUse = InventoryItemDatas[i].bCanUse;
		//ViewItemDatas[i].Icon = InventoryItemDatas[i].Icon;
	}
}



void UDDInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, ViewItemDatas);
}

void UDDInventoryComponent::OnRep_ClientViewItemData()
{
	OnInventoryChanged.Broadcast();
}


FItemTableRow* UDDInventoryComponent::GetItemData(FName RowName) const
{
	if (!ItemDataTable) return nullptr;
	return ItemDataTable->FindRow<FItemTableRow>(RowName, TEXT("Item"));
}

void UDDInventoryComponent::Server_AddItem_Implementation(FName ItemName)
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
			LOG_PMJ(Warning, TEXT("====== 아이템 갯수 + 1 ======"));
			InventoryItemData.ItemCount++;
			RefreshInventory();
		}
	}
}

void UDDInventoryComponent::Server_UseItem_Implementation(const FName& ItemSlotName)
{
	if (ItemSlotName.IsNone()) return;
	for (FInventoryItemData& ItemDatas : InventoryItemDatas)
	{
		if (ItemDatas.ItemName == ItemSlotName)
		{
			LOG_PMJ(Error, TEXT("====== 사용아이템존재 ======"));
			ADDBoardGameCharacter* Character = Cast<ADDBoardGameCharacter>(OwningController.Get()->GetPawn());
			if (!IsValid(Character)) return;
			UItemActionComponent* ISC = Character->FindComponentByClass<UItemActionComponent>();
			if (!IsValid(ISC)) return;
			LOG_PMJ(Error, TEXT("====== 아이템컴포넌트존재 ======"));
			FItemTableRow& CurrentItemDataRow = *GetItemData(ItemDatas.ItemName);
			ISC->BeginItemAction(CurrentItemDataRow);
			ItemDatas.ItemCount--;
			RefreshInventory();
		}
	}
	OwningController->Client_CloseInventory();
}
FName UDDInventoryComponent::AddRandomItem()
{
	if (!GetOwner()->HasAuthority()) return NAME_None;
	if (!ItemDataTable) return NAME_None;

	const TArray<FName> RowNames = ItemDataTable->GetRowNames();
	if (RowNames.Num() == 0) return NAME_None;

	int32 Index = FMath::RandRange(0, RowNames.Num() - 1);
	FName RandomItem = RowNames[Index];

	Server_AddItem(RandomItem);

	return RandomItem;
}
