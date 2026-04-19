
#include "UI/Inventory/DDInventoryComponent.h"
#include "Base/Character/DDBaseCharacter.h"
#include "Base/Player/DDBasePlayerController.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "BoardGame/Character/Components/ItemActionComponent.h"
#include "Common/DDLogManager.h"
#include "Data/DDItemDataTypes.h"
#include "Net/UnrealNetwork.h"
#include "System/DDGameplayTags.h"


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
		Server_InitializeInventoryData();
		Server_AddItem("HealingKit");
		Server_AddItem("Magnet");
	}
	
	OwningController = Cast<ADDBasePlayerController>(GetOwner());
}

void UDDInventoryComponent::Server_InitializeInventoryData_Implementation()
{
	if (!(GetOwner()->HasAuthority()))
	{
		LOG_PMJ(Error, TEXT("Server_InitializeInventoryData_Implementation 1"));
		return;	
	}
	if (!ItemDataTable)
	{
		LOG_PMJ(Error, TEXT("Server_InitializeInventoryData_Implementation 2"));
		return;
	}
	const TArray<FName>& AllItemNames = ItemDataTable->GetRowNames();
	if (AllItemNames.IsEmpty())
	{
		LOG_PMJ(Error, TEXT("Server_InitializeInventoryData_Implementation 3"));
		return;
	}
	for (int32 i = 0; i < AllItemNames.Num(); ++i)
	{
		FItemTableRow* ItemDataRow = ItemDataTable->FindRow<FItemTableRow>(AllItemNames[i], TEXT("FindItem"));
		if (!ItemDataRow)
		{
			LOG_PMJ(Error, TEXT("Server_InitializeInventoryData_Implementation 4"));
			continue;
		}
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
	LOG_PMJ(Warning, TEXT("ServerRPCAddItem 진입"));
	if (!GetOwner()->HasAuthority()) return;
	
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
			
			ADDBoardGameCharacter* Character = Cast<ADDBoardGameCharacter>(OwningController->GetCharacter());
			if (!IsValid(Character)) return;
			UItemActionComponent* IAC = Character->FindComponentByClass<UItemActionComponent>();
			if (!IsValid(IAC)) return;
			FItemTableRow& CurrentItemDataRow = *GetItemData(ItemDatas.ItemName);
			IAC->BeginItemAction(CurrentItemDataRow);
			ItemDatas.ItemCount--;
			RefreshInventory();
		}
	}
	OwningController->Client_ClosePopUp(DDGameplayTags::BoardGame_UI_Inventory);
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
