#include "BoardGame/Character/Components/ItemActionComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "BoardGame/Game/DDBoardGameMode.h"
#include "Common/DDLogManager.h"
#include "Data/DDItemDataTypes.h"
#include "System/DDGameplayTags.h"

UItemActionComponent::UItemActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UItemActionComponent::BeginItemAction(const FItemTableRow& ItemRow)
{
	if (IsItemActionActive())
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] 이미 아이템 액션 중입니다."));
		return;
	}
	
	ActiveItemID = ItemRow.ItemID;
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
	
	ResetItemAction();
}

void UItemActionComponent::ConfirmItemAction()
{
	if (!ActiveItemAbility)
	{
		CancelItemAction();
		return;
	}
	
	switch (CurrentActionMode)
	{
	case EItemActionMode::Instant:
		Server_ConfirmInstantItem(ActiveItemID);
		ResetItemAction();
		break;

	case EItemActionMode::Targeting:
		if (AActor* TargetActor = GetSelectedTarget())
		{
			Server_ConfirmTargetingItem(ActiveItemID, TargetActor);
			ResetItemAction();
		}
		break;

	case EItemActionMode::Range:
		Server_ConfirmRangeItem(ActiveItemID);
		ResetItemAction();
		break;
	default:
		break;
	}
}

void UItemActionComponent::CancelItemAction()
{
	LOG_JJH(Warning, TEXT("[아이템 액션] 취소 : %s"), *ActiveItemID.ToString());
	
	Server_FocusItemTarget(GetOwner());
	
	// TODO 범위표시가 있다면 제거하고, 다시 인벤토리 창 띄우기....	
	
	ResetItemAction();
}

void UItemActionComponent::Server_FocusItemTarget_Implementation(AActor* TargetActor)
{
	if (!IsValid(TargetActor))
	{
		return;
	}

	ADDBoardGameMode* BoardGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ADDBoardGameMode>() : nullptr;
	if (!BoardGameMode)
	{
		return;
	}

	BoardGameMode->FocusAllCamerasOnTarget(TargetActor);
}

void UItemActionComponent::Server_ConfirmInstantItem_Implementation(FName ItemID)
{
	LOG_JJH(Warning, TEXT("[아이템 액션] 즉시사용 아이템 액션 : %s"), *ItemID.ToString());
	//TODO 사용까지 구현
}

void UItemActionComponent::Server_ConfirmTargetingItem_Implementation(FName ItemID, AActor* TargetActor)
{
	LOG_JJH(Warning, TEXT("[아이템 액션] 타게팅 아이템 액션 : %s, Target: %s"), *ItemID.ToString(), *GetNameSafe(TargetActor));
	//TODO 사용까지 구현
}

void UItemActionComponent::Server_ConfirmRangeItem_Implementation(FName ItemID)
{
	LOG_JJH(Warning, TEXT("[아이템 액션] 범위 아이템 액션 : %s"), *ItemID.ToString());
	//TODO 사용까지 구현
}

void UItemActionComponent::StartInstantAction()
{
	CurrentActionMode = EItemActionMode::Instant;
	ConfirmItemAction();
}

void UItemActionComponent::StartTargetingAction()
{
	
	LOG_JJH(Warning, TEXT("[아이템 액션] 타게팅 액션 시작"));
	
	CurrentActionMode = EItemActionMode::Targeting;
	ApplyItemActionTag();
	SelectedTargetIndex = INDEX_NONE;
	
	BuildTargetCandidates();
	
	LOG_JJH(Warning, TEXT("[아이템 액션] 타겟 후보 수 : %d"), CandidateTargets.Num());
	
	if (CandidateTargets.IsEmpty())
	{
		CancelItemAction();
		return;
	}
	
	// 첫 번째 타겟을 한 번 지정.
	SelectedTargetIndex = 0;
	Server_FocusItemTarget(GetSelectedTarget());
}

void UItemActionComponent::StartRangeAction()
{
	CurrentActionMode = EItemActionMode::Range;
	ApplyItemActionTag();
	
	// TODO 범위를 표시하고, 해당 범위 내의 플레이어를 순회해서 Owner와 비교한 뒤 후보자에 추가
}

void UItemActionComponent::BuildTargetCandidates()
{
	CandidateTargets.Reset();
	
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	AActor* OwnerCharacter = GetOwner();
	if (!OwnerCharacter)
	{
		return;
	}
	
	// 월드의 플레이어 컨트롤러를 순회해서 해당 컨트롤러의 Pawn을 OwnerCharacter와 비교한 뒤 다른 Pawn만 후보자로 추가
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		if (!PlayerController)
		{
			continue;
		}
		
		ADDBoardGameCharacter* CandidateCharacter = Cast<ADDBoardGameCharacter>(PlayerController->GetPawn());
		if (!CandidateCharacter)
		{
			continue;
		}
		
		if (CandidateCharacter == OwnerCharacter)
		{
			continue;
		}
		
		CandidateTargets.Add(CandidateCharacter);
	}
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
	Server_FocusItemTarget(GetSelectedTarget());
}

AActor* UItemActionComponent::GetSelectedTarget() const
{
	return CandidateTargets.IsValidIndex(SelectedTargetIndex)
		       ? CandidateTargets[SelectedTargetIndex].Get()
		       : nullptr;
}

void UItemActionComponent::ResetItemAction()
{
	RemoveItemActionTag();
	
	CurrentActionMode = EItemActionMode::None;
	ActiveItemID = NAME_None;
	ActiveItemType = FGameplayTag();
	ActiveItemAbility = nullptr;
	CandidateTargets.Reset();
	SelectedTargetIndex = INDEX_NONE;
}

void UItemActionComponent::ApplyItemActionTag()
{
	ADDBoardGameCharacter* OwnerCharacter = Cast<ADDBoardGameCharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}
	
	UAbilitySystemComponent* AbilitySystemComp = OwnerCharacter->GetAbilitySystemComponent();
	if (!AbilitySystemComp)
	{
		return;
	}
	
	if (CurrentActionMode == EItemActionMode::Targeting)
	{
		AbilitySystemComp->AddLooseGameplayTag(DDGameplayTags::State_ItemAction_Targeting);
	}
	else if (CurrentActionMode == EItemActionMode::Range)
	{
		AbilitySystemComp->AddLooseGameplayTag(DDGameplayTags::State_ItemAction_Range);
	}
}

void UItemActionComponent::RemoveItemActionTag()
{
	ADDBoardGameCharacter* OwnerCharacter = Cast<ADDBoardGameCharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}
	
	UAbilitySystemComponent* AbilitySystemComp = OwnerCharacter->GetAbilitySystemComponent();
	if (!AbilitySystemComp)
	{
		return;
	}
	
	AbilitySystemComp->RemoveLooseGameplayTag(DDGameplayTags::State_ItemAction_Targeting);
	AbilitySystemComp->RemoveLooseGameplayTag(DDGameplayTags::State_ItemAction_Range);
}
