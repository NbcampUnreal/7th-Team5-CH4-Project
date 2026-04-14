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
	
	if (ActiveItemType == DDGameplayTags::Item_Activate_Instant)
	{
		StartInstantAction();
		return;
	}

	if (ActiveItemType == DDGameplayTags::Item_Activate_Targeting)
	{
		StartTargetingAction();
		return;
	}

	if (ActiveItemType == DDGameplayTags::Item_Activate_Range)
	{
		StartRangeAction();
		return;
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
	
	// TODO 범위를 표시하고, 해당 범위 내의 플레이어를 순회해서 Owner와 비교한 뒤 후보자에 추가
}

void UItemActionComponent::BuildTargetCandidates()
{
	// TODO 타겟 후보 만들기. 범위에 따라 너무 멀면 타겟 후보에서 제외하는 로직이 필요할 것 같음.
}

void UItemActionComponent::SelectNextTarget()
{
	ChangeTarget(1);
}

void UItemActionComponent::SelectPreviousTarget()
{
	ChangeTarget(-1);
}

void UItemActionComponent::ChangeTarget(int32 Offset)
{
	if (CurrentActionMode != EItemActionMode::Targeting)
	{
		return;
	}
	
	if (CandidateTargets.IsEmpty())
	{
		return;
	}
	
	// 목록을 원형으로 순환하기 위한 계산 : (현재인덱스 + 방향 + 목록사이즈) % 목록사이즈
	// -1 방향으로도 가기 때문에 Index가 음수가 되지 않기 위해서 사이즈를 한 번 더함
	SelectedTargetIndex = (SelectedTargetIndex + Offset + CandidateTargets.Num()) % CandidateTargets.Num();
}

void UItemActionComponent::ResetItemAction()
{
	CurrentActionMode = EItemActionMode::None;
	ActiveItemID = NAME_None;
	ActiveItemType = FGameplayTag();
	ActiveItemAbility = nullptr;
	CandidateTargets.Reset();
	SelectedTargetIndex = INDEX_NONE;
}
