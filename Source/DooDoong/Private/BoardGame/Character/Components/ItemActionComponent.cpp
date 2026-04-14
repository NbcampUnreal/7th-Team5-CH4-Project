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
	
	switch (ActiveItemType)
	{
	case DDGameplayTags::Item_Activate_Instant:
		StartInstantAction();
		return;
	case DDGameplayTags::Item_Activate_Targeting:
		StartTargetingAction();
		return;
	case DDGameplayTags::Item_Activate_Range:
		StartRangeAction();
		return;
	default:
		break;
	}
	
	CancelItemAction();
}

void UItemActionComponent::ConfirmItemAction()
{
	switch (CurrentActionMode)
	{
	case EItemActionMode::Instant:
		// 인스턴트 아이템 확정을 서버에 요청해야할듯. Ability가 Attribute를 변화시킴. ServerOnly.
		break;

	case EItemActionMode::Targeting:
		// 
		break;

	case EItemActionMode::Range:
		//
		break;
	default:
		break;
	}
}

void UItemActionComponent::CancelItemAction()
{
	// TODO 아이템 사용을 Cancel
	
	// TODO 다시 인벤토리 창 띄우기
}

void UItemActionComponent::StartInstantAction()
{
	CurrentActionMode = EItemActionMode::Instant;
	ConfirmItemAction();
}

void UItemActionComponent::StartTargetingAction()
{
	CurrentActionMode = EItemActionMode::Targeting;
	
	CandidateTargets.Reset();
	SelectedTargetIndex = INDEX_NONE;
	
	//TODO 모든 플레이어 컨트롤러슬 순회해서 Owner와 비교해서 다르면 후보자로 추가해야할 것 같음.
	
	if (CandidateTargets.IsEmpty())
	{
		CancelItemAction();
		return;
	}
	
	// 첫 번째 타겟을 한 번 지정.
	SelectedTargetIndex = 0;
}

void UItemActionComponent::StartRangeAction()
{
	CurrentActionMode = EItemActionMode::Range;
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