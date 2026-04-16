#include "BoardGame/Character/Components/ItemActionComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "Common/DDLogManager.h"
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
		CurrentActionMode = EItemActionMode::Instant;
		ConfirmCurrentItemAction();
		return;
	}

	if (ActiveItemTag == DDGameplayTags::Item_Activate_Targeting)
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] 타게팅 액션 시작"));

		CurrentActionMode = EItemActionMode::Targeting;
		ApplyItemActionTag();
		Server_ActivateItemAbility(ActiveItemID, ActiveItemAbility, nullptr);
		return;
	}

	if (ActiveItemTag == DDGameplayTags::Item_Activate_Range)
	{
		CurrentActionMode = EItemActionMode::Range;
		ApplyItemActionTag();
		// TODO 범위를 표시하고, 해당 범위 내의 플레이어를 순회해서 Owner와 비교한 뒤 후보자에 추가
		return;
	}
	
	ResetItemAction();
}

void UItemActionComponent::ConfirmCurrentItemAction()
{
	if (!ActiveItemAbility)
	{
		CancelCurrentItemAction();
		return;
	}
	
	switch (CurrentActionMode)
	{
	case EItemActionMode::Instant:
		// 서버에서 아이템 어빌리티를 활성화하도록 요청
		Server_ActivateItemAbility(ActiveItemID, ActiveItemAbility, nullptr);
		ResetItemAction();
		break;

	case EItemActionMode::Targeting:
		SendTargetingInput(EItemTargetingInput::Confirm);
		break;

	case EItemActionMode::Range:
		Server_ActivateItemAbility(ActiveItemID, ActiveItemAbility, nullptr);
		ResetItemAction();
		break;
	default:
		break;
	}
}

void UItemActionComponent::CancelCurrentItemAction()
{
	LOG_JJH(Warning, TEXT("[아이템 액션] 취소 : %s"), *ActiveItemID.ToString());

	if (CurrentActionMode == EItemActionMode::Targeting)
	{
		Server_SendTargetingInputEvent(DDGameplayTags::Event_Item_Target_Cancel);
	}
	
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

void UItemActionComponent::Server_ActivateItemAbility_Implementation(FName ItemID, TSubclassOf<UGameplayAbility> ItemAbility, AActor* TargetActor)
{
	LOG_JJH(Warning, TEXT("[아이템 액션] 아이템 Ability 실행 요청 : %s, Target: %s"), *ItemID.ToString(), *GetNameSafe(TargetActor));
	
	// 아이템 실행
	if (!TryGiveAndActivateItemAbility(ItemID, ItemAbility, TargetActor))
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] 아이템 Ability 실행 실패 : %s"), *ItemID.ToString());
		return;
	}
}

void UItemActionComponent::SendTargetingInput(EItemTargetingInput Input)
{
	if (CurrentActionMode != EItemActionMode::Targeting)
	{
		return;
	}

	if (Input == EItemTargetingInput::Cancel)
	{
		CancelCurrentItemAction();
		return;
	}

	FGameplayTag EventTag;
	switch (Input)
	{
	case EItemTargetingInput::Next:
		EventTag = DDGameplayTags::Event_Item_Target_Next;
		break;
	case EItemTargetingInput::Previous:
		EventTag = DDGameplayTags::Event_Item_Target_Previous;
		break;
	case EItemTargetingInput::Confirm:
		EventTag = DDGameplayTags::Event_Item_Target_Confirm;
		break;
	default:
		return;
	}

	Server_SendTargetingInputEvent(EventTag);

	if (Input == EItemTargetingInput::Confirm)
	{
		ResetItemAction();
	}
}

void UItemActionComponent::Server_SendTargetingInputEvent_Implementation(FGameplayTag EventTag)
{
	AActor* OwnerActor = GetOwner();
	if (!EventTag.IsValid() || !IsValid(OwnerActor))
	{
		return;
	}

	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.Instigator = OwnerActor;
	Payload.OptionalObject = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, Payload);
}

bool UItemActionComponent::TryGiveAndActivateItemAbility(FName ItemID, TSubclassOf<UGameplayAbility> ItemAbility, AActor* TargetActor)
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
	
	// 아이템 확성화 이벤트 태그를 발송
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
