#include "UI/Inventory/DDInventoryComponent.h"
#include "Base/Character/DDBaseCharacter.h"
#include "Base/Player/DDBasePlayerController.h"
#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/Character/Components/ItemActionComponent.h"
#include "Common/DDLogManager.h"
#include "Data/DDItemDataTypes.h"
#include "Net/UnrealNetwork.h"
#include "System/DDGameInstance.h"


UDDInventoryComponent::UDDInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	
}

ADDBasePlayerState* UDDInventoryComponent::CashingPlayerState() const
{
	ADDBasePlayerState* DDPS = Cast<ADDBasePlayerState>(GetOwner());
	if (!IsValid(DDPS)) return nullptr;
	
	return DDPS;
}

void UDDInventoryComponent::Server_InitializeInventoryData_Implementation()
{
	if (GetOwner()->HasAuthority() && CashingPlayerState()->bInitializeInventory == false)
	{
		LOG_PMJ(Warning, TEXT("인벤토리 초기화 호출"));
		
		LOG_PMJ(Warning, TEXT("Server_InitializeInventoryData_Implementation 1"));
		
		if (!UDDGameInstance::Get(GetWorld())->ItemDataTable)
		{
			LOG_PMJ(Warning, TEXT("Server_InitializeInventoryData_Implementation 2"));
			return;
		}
		const TArray<FName>& AllItemNames = UDDGameInstance::Get(GetWorld())->ItemDataTable->GetRowNames();
		if (AllItemNames.IsEmpty())
		{
			LOG_PMJ(Warning, TEXT("Server_InitializeInventoryData_Implementation 3"));
			return;
		}
		for (int32 i = 0; i < AllItemNames.Num(); ++i)
		{
			FItemTableRow* ItemDataRow = UDDGameInstance::Get(GetWorld())->ItemDataTable->FindRow<FItemTableRow>(AllItemNames[i], TEXT("FindItem"));
			if (!ItemDataRow)
			{
				LOG_PMJ(Warning, TEXT("Server_InitializeInventoryData_Implementation 4"));
				continue;
			}
		
			FInventoryItemData InventoryItemData;
			InventoryItemData.ItemName = AllItemNames[i];
			InventoryItemData.ItemCount = 0;
			InventoryItemData.bCanUse = false;
			InventoryItemData.Icon = ItemDataRow->Icon;
			CashingPlayerState()->InventoryItemDatas.Add(InventoryItemData);
			
			FViewItemData ViewItemData;
			ViewItemData.ViewItemName = AllItemNames[i];
			ViewItemData.ViewItemCount = 0;
			ViewItemData.bCanUse = false;
			ViewItemData.Icon = ItemDataRow->Icon;
			ViewItemDatas.Add(ViewItemData);
		}
		CashingPlayerState()->bInitializeInventory = true;
	}
	RefreshInventory();
}

void UDDInventoryComponent::RefreshInventory()
{
	
	if (CashingPlayerState()->InventoryItemDatas.IsEmpty())
	{
		LOG_PMJ(Warning, TEXT("===== 원본 인벤토리 데이터가 비어있습니다 ====="));
		return;
	}
	if (ViewItemDatas.Num() != CashingPlayerState()->InventoryItemDatas.Num())
	{
		ViewItemDatas.SetNum(CashingPlayerState()->InventoryItemDatas.Num());
	}
	for (int32 i = 0; i < CashingPlayerState()->InventoryItemDatas.Num(); ++i)
	{
		ViewItemDatas[i].ViewItemName = CashingPlayerState()->InventoryItemDatas[i].ItemName;
		ViewItemDatas[i].ViewItemCount = CashingPlayerState()->InventoryItemDatas[i].ItemCount;
		ViewItemDatas[i].bCanUse = CashingPlayerState()->InventoryItemDatas[i].bCanUse;
		ViewItemDatas[i].Icon = CashingPlayerState()->InventoryItemDatas[i].Icon;
	}
}

void UDDInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, ViewItemDatas);
}

void UDDInventoryComponent::SetOwningController(ADDBasePlayerController* PC)
{
	OwningController = PC;
}

void UDDInventoryComponent::OnRep_ClientViewItemData()
{
	OnInventoryChanged.Broadcast();
}


FItemTableRow* UDDInventoryComponent::GetItemData(FName RowName) const
{
	if (!UDDGameInstance::Get(GetWorld())->ItemDataTable) return nullptr;
	return UDDGameInstance::Get(GetWorld())->ItemDataTable->FindRow<FItemTableRow>(RowName, TEXT("Item"));
}

void UDDInventoryComponent::Server_AddItem_Implementation(FName ItemName)
{
	LOG_PMJ(Warning, TEXT("ServerRPCAddItem 진입"));
	
	if (!GetOwner()->HasAuthority()) return;
	
	if (CashingPlayerState()->InventoryItemDatas.IsEmpty())
	{
		LOG_PMJ(Warning, TEXT("인벤데이터가 비어있습니다"));
		return;
	}
	
	for (FInventoryItemData& InventoryItemData : CashingPlayerState()->InventoryItemDatas)
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
	if (ItemSlotName.IsNone())
	{
			LOG_PMJ(Warning, TEXT("====== 아이템 이름 없음 ======"));
	}
	for (FInventoryItemData& ItemData : CashingPlayerState()->InventoryItemDatas)
	{
		if (ItemData.ItemName == ItemSlotName)
		{
			
			ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(CashingPlayerState()->GetPlayerController());
			if (DDPC == nullptr)
			{
				LOG_PMJ(Warning, TEXT(" UseItem: 컨트롤러 캐스팅 실패 "));
			}
			
			ADDBaseCharacter* DDCharacter = Cast<ADDBaseCharacter>(DDPC->GetCharacter());
			if (DDCharacter == nullptr)
			{
				LOG_PMJ(Warning, TEXT(" UseItem: 캐릭터 캐스팅 실패 "));
			}
			
			UItemActionComponent* IAC = DDCharacter->FindComponentByClass<UItemActionComponent>();
			if (!IsValid(IAC))
			{
				LOG_PMJ(Warning, TEXT(" UseItem: 아이템 엑션 컴포넌트 찾기 실패 "));
				return;
			}
			
			FItemTableRow* CurrentItemDataRow = GetItemData(ItemData.ItemName);  // ← 포인터로 받기
			if (!CurrentItemDataRow)
			{
				LOG_PMJ(Warning, TEXT(" UseItem: 아이템 데이터 불러오기 실패 "));
				return;
			}
			
			IAC->BeginItemAction(*CurrentItemDataRow);
			ItemData.ItemCount--;
			RefreshInventory();
		}
	}
	//OwningController->Client_ClosePopUp(DDGameplayTags::BoardGame_UI_Inventory);
}

FName UDDInventoryComponent::AddRandomItem()
{
	if (!GetOwner()->HasAuthority()) return NAME_None;
	if (!UDDGameInstance::Get(GetWorld())->ItemDataTable) return NAME_None;

	const TArray<FName> RowNames = UDDGameInstance::Get(GetWorld())->ItemDataTable->GetRowNames();
	if (RowNames.Num() == 0) return NAME_None;

	int32 Index = FMath::RandRange(0, RowNames.Num() - 1);
	FName RandomItem = RowNames[Index];

	Server_AddItem(RandomItem);

	return RandomItem;
}
