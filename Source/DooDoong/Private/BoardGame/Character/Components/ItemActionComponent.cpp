#include "BoardGame/Character/Components/ItemActionComponent.h"

#include "GameplayTagContainer.h"
#include "Data/DDItemDataTypes.h"
#include "System/DDGameplayTags.h"



UItemActionComponent::UItemActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UItemActionComponent::BeginItemAction(const FName ItemID)
{
	if (!ItemData)
	{
		return;
	}

	FItemTableRow* Row = ItemData->FindRow<FItemTableRow>(ItemID, TEXT(""));
	if (!Row)
	{
		return;
	}
	
	if (Row->ItemType == DDGameplayTags::Item_Activate_Instant)
	{
		ConfirmItemAction();
	}
	else
	{
		TryActivateItem();
	}
}

void UItemActionComponent::TryActivateItem()
{
	// TODO 키 입력에 따른 카메라이동 구현. (이건 Tick???) 
	
	
}

void UItemActionComponent::SelectNextTarget()
{
	// TODO E를 누르면 다음 타겟으로 이동. 이동 텀이 있어야함. 0.5f초임
}

void UItemActionComponent::SelectPreviousTarget()
{
	// TODO Q를 누르면 이전 타겟으로 이동
}

void UItemActionComponent::ConfirmItemAction()
{
	// TODO 아이템 사용을 Confirm
}

void UItemActionComponent::CancelItemAction()
{
	// TODO 아이템 사용을 Cancel
	
	// TODO 다시 인벤토리 창 띄우기
}
