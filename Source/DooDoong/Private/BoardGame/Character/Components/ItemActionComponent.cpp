#include "BoardGame/Character/Components/ItemActionComponent.h"

#include "GameplayTagContainer.h"
#include "Data/DDItemDataTypes.h"
#include "System/DDGameplayTags.h"



UItemActionComponent::UItemActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UItemActionComponent::BeginItemAction(FName ItemID, const FItemTableRow& ItemRow)
{
	ActiveItemID = ItemID;
	ActiveItemType = ItemRow.ItemType;
	ActiveItemAbility = ItemRow.ItemAbility;
	
	switch (CurrentActionMode)
	{
	case EItemActionMode::Instant:
		CurrentActionMode = EItemActionMode::Instant;
		ConfirmItemAction();
		return;
	case EItemActionMode::Targeting:
		CurrentActionMode = EItemActionMode::Targeting;
		TryActivateItem();
		return;
	case EItemActionMode::Range:
		CurrentActionMode = EItemActionMode::Range;
		TryActivateItem();
		return;
	default:
		break;
	}
	
	CancelItemAction();
}

void UItemActionComponent::TryActivateItem()
{
	if (CurrentActionMode == EItemActionMode::Targeting)
	{
		//TODO 타게팅 관련 카메라이동 모드 활성화
		
		return;
	}
	
	if (CurrentActionMode == EItemActionMode::Range)
	{
		//TODO 범위 표시 관련 모드 활성화
		
		return;
	}
	
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

void UItemActionComponent::BuildTargetCandidates()
{
	// TODO 타겟 후보 만들기. 범위에 따라 너무 멀면 타겟 후보에서 제외하는 로직이 필요할 것 같음.
}

void UItemActionComponent::SelectNextTarget()
{
	// TODO E를 누르면 다음 타겟으로 이동. 이동 텀이 있어야함. 0.5f초
}

void UItemActionComponent::SelectPreviousTarget()
{
	// TODO Q를 누르면 이전 타겟으로 이동
}