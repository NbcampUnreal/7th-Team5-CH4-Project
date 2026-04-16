#include "BoardGame/Character/Components/ItemActionComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "BoardGame/Game/DDBoardGameMode.h"
#include "Common/DDLogManager.h"
#include "Data/DDItemDataTypes.h"
#include "System/DDGameplayTags.h"
#include "UI/Inventory/DDInventoryComponent.h"

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
	ActiveItemTag = ItemRow.ItemTag;
	ActiveItemAbility = ItemRow.ItemAbility;
	
	if (ActiveItemTag == DDGameplayTags::Item_Activate_Instant)
	{
		StartInstantAction();
		return;
	}

	if (ActiveItemTag == DDGameplayTags::Item_Activate_Targeting)
	{
		StartTargetingAction();
		return;
	}

	if (ActiveItemTag == DDGameplayTags::Item_Activate_Range)
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
		Server_ConfirmInstantItem(ActiveItemID, ActiveItemAbility);
		ResetItemAction();
		break;

	case EItemActionMode::Targeting:
		if (AActor* TargetActor = GetSelectedTarget())
		{
			Server_ConfirmTargetingItem(ActiveItemID, ActiveItemAbility, TargetActor);
			ResetItemAction();
		}
		break;

	case EItemActionMode::Range:
		Server_ConfirmRangeItem(ActiveItemID, ActiveItemAbility);
		ResetItemAction();
		break;
	default:
		break;
	}
}

void UItemActionComponent::CancelItemAction()
{
	LOG_JJH(Warning, TEXT("[아이템 액션] 취소 : %s"), *ActiveItemID.ToString());
	
	// 카메라 원상복구
	Server_FocusItemTarget(GetOwner());
	// 아이템 수 원상복구
	RestoreCanceledItem(ActiveItemID);
	// 다시 인벤토리 창 띄우기
	if (const ADDBoardGameCharacter* OwnerCharacter = Cast<ADDBoardGameCharacter>(GetOwner()))
	{
		if (AController* OwnerController = OwnerCharacter->GetController())
		{
			if (UDDInventoryComponent* InventoryComponent = OwnerController->FindComponentByClass<UDDInventoryComponent>())
			{
				InventoryComponent->RequestOpenInventory();
			}
		}
	}
	
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

void UItemActionComponent::Server_ConfirmInstantItem_Implementation(FName ItemID, TSubclassOf<UGameplayAbility> ItemAbility)
{
	LOG_JJH(Warning, TEXT("[아이템 액션] 즉시사용 아이템 액션 : %s"), *ItemID.ToString());
	
	if (!TryActivateItemAbility(ItemID, ItemAbility, nullptr))
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] 즉시사용 아이템 Ability 실행 실패 : %s"), *ItemID.ToString());
		return;
	}
}

void UItemActionComponent::Server_ConfirmTargetingItem_Implementation(FName ItemID, TSubclassOf<UGameplayAbility> ItemAbility, AActor* TargetActor)
{
	LOG_JJH(Warning, TEXT("[아이템 액션] 타게팅 아이템 액션 : %s, Target: %s"), *ItemID.ToString(), *GetNameSafe(TargetActor));
	
	if (!IsValid(TargetActor))
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] 타게팅 아이템 TargetActor가 유효하지 않습니다. ItemID: %s"), *ItemID.ToString());
		return;
	}
	
	if (!TryActivateItemAbility(ItemID, ItemAbility, TargetActor))
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] 타게팅 아이템 Ability 실행 실패 : %s"), *ItemID.ToString());
		return;
	}
}

void UItemActionComponent::Server_ConfirmRangeItem_Implementation(FName ItemID, TSubclassOf<UGameplayAbility> ItemAbility)
{
	LOG_JJH(Warning, TEXT("[아이템 액션] 범위 아이템 액션 : %s"), *ItemID.ToString());
	
	if (!TryActivateItemAbility(ItemID, ItemAbility, nullptr))
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] 범위 아이템 Ability 실행 실패 : %s"), *ItemID.ToString());
		return;
	}
	
	// TODO 범위 내 대상 목록을 EventData에 담는 방식이 정해지면 TargetActor 대신 TargetData 구조로 확장
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
	for (TActorIterator<ADDBoardGameCharacter> It(World); It; ++It)
	{
		ADDBoardGameCharacter* CandidateCharacter = *It;
		if (!CandidateCharacter || CandidateCharacter == OwnerCharacter)
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

void UItemActionComponent::SendTargetingInputEvent(FGameplayTag EventTag)
{
	if (!EventTag.IsValid())
	{
		return;
	}

	AActor* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor))
	{
		return;
	}

	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.Instigator = OwnerActor;
	Payload.OptionalObject = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, Payload);
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

bool UItemActionComponent::TryActivateItemAbility(FName ItemID, TSubclassOf<UGameplayAbility> ItemAbility, AActor* TargetActor)
{
	if (!ItemAbility)
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] ItemAbility가 유효하지 않습니다. ItemID: %s"), *ItemID.ToString());
		return false;
	}
	
	ADDBoardGameCharacter* OwnerCharacter = Cast<ADDBoardGameCharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] OwnerCharacter가 유효하지 않습니다. ItemID: %s"), *ItemID.ToString());
		return false;
	}
	
	UAbilitySystemComponent* AbilitySystemComp = OwnerCharacter->GetAbilitySystemComponent();
	if (!AbilitySystemComp)
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] AbilitySystemComponent가 유효하지 않습니다. ItemID: %s"), *ItemID.ToString());
		return false;
	}
	
	// 타게팅 아이템 Ability는 EventData->Target에서 선택 대상을 읽으면 되는 방식.
	FGameplayEventData EventData;
	EventData.EventTag = DDGameplayTags::Event_Item_Activate;
	EventData.Instigator = OwnerCharacter;
	EventData.Target = TargetActor;
	EventData.OptionalObject = this;
	
	FGameplayAbilitySpec AbilitySpec(ItemAbility, 1, INDEX_NONE, this);
	// 아이템 Ability는 사용 시점에 1회성으로 부여하고 즉시 실행
	const FGameplayAbilitySpecHandle ActivatedHandle = AbilitySystemComp->GiveAbilityAndActivateOnce(AbilitySpec, &EventData);
	if (!ActivatedHandle.IsValid())
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] GiveAbilityAndActivateOnce 실패. ItemID: %s, Ability: %s"),
		        *ItemID.ToString(),
		        *GetNameSafe(ItemAbility.Get()));
		return false;
	}
	
	LOG_JJH(Warning, TEXT("[아이템 액션] Ability 실행 성공. ItemID: %s, Ability: %s, Target: %s"),
	        *ItemID.ToString(),
	        *GetNameSafe(ItemAbility.Get()),
	        *GetNameSafe(TargetActor));
	
	return true;
}

void UItemActionComponent::RestoreCanceledItem(FName ItemID)
{
	if (ItemID.IsNone())
	{
		return;
	}
	
	const ADDBoardGameCharacter* OwnerCharacter = Cast<ADDBoardGameCharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] 취소 아이템 복구 실패: OwnerCharacter가 유효하지 않습니다. ItemID: %s"), *ItemID.ToString());
		return;
	}
	
	AController* OwnerController = OwnerCharacter->GetController();
	if (!OwnerController)
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] 취소 아이템 복구 실패: Controller가 유효하지 않습니다. ItemID: %s"), *ItemID.ToString());
		return;
	}
	
	UDDInventoryComponent* InventoryComponent = OwnerController->FindComponentByClass<UDDInventoryComponent>();
	if (!InventoryComponent)
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] 취소 아이템 복구 실패: InventoryComponent가 유효하지 않습니다. ItemID: %s"), *ItemID.ToString());
		return;
	}
	
	// InventoryComponent가 BeginItemAction 호출 전에 수량을 1개 선차감하므로, 취소 시 다시 1개를 더한다.
	InventoryComponent->AddItem(ItemID);
	LOG_JJH(Warning, TEXT("[아이템 액션] 취소 아이템 복구 : %s"), *ItemID.ToString());
}

void UItemActionComponent::ResetItemAction()
{
	RemoveItemActionTag();
	
	CurrentActionMode = EItemActionMode::None;
	ActiveItemID = NAME_None;
	ActiveItemTag = FGameplayTag();
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
