#include "BoardGame/Character/Components/ItemActionComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Base/Player/DDBasePlayerController.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "Common/DDLogManager.h"
#include "Data/ItemPayloadObject.h"
#include "Net/UnrealNetwork.h"
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
	ItemRowData = ItemRow;
	ActiveItemID = ItemRow.ItemID;
	ActiveItemTag = ItemRow.ItemTag;
	ActiveItemAbilityTag = ItemRow.ItemAbilityTag;

	// 현재 액션 모드 지정
	if (ActiveItemTag == DDGameplayTags::Item_Activate_Instant)
		CurrentActionMode = EItemActionMode::Instant;
	else if (ActiveItemTag == DDGameplayTags::Item_Activate_Targeting)
		CurrentActionMode = EItemActionMode::Targeting;
	else if (ActiveItemTag == DDGameplayTags::Item_Activate_Range)
		CurrentActionMode = EItemActionMode::Range;
	else
	{
		ResetItemAction();
		return;
	}
	
	if (CurrentActionMode != EItemActionMode::Instant)
	{
		ApplyItemActionTag();
	}
	// 어빌리티 실행
	Server_ActivateItemAbility(ActiveItemID, ActiveItemAbilityTag, nullptr);
	
	if (CurrentActionMode == EItemActionMode::Instant)
	{
		ResetItemAction();
	}
}

// 컨펌 입력 액션
void UItemActionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentActionMode);
	DOREPLIFETIME(ThisClass, ActiveItemID);
	DOREPLIFETIME(ThisClass, ActiveItemTag);
	DOREPLIFETIME(ThisClass, ActiveItemAbilityTag);
}

void UItemActionComponent::ConfirmCurrentItemAction()
{
	if (!ActiveItemAbilityTag.IsValid())
	{
		CancelCurrentItemAction();
		return;
	}
	
	switch (CurrentActionMode)
	{
	case EItemActionMode::Targeting:
		SendTargetingInput(EItemTargetingInput::Confirm);
		break;

	case EItemActionMode::Range:
		Server_SendInputEvent(DDGameplayTags::Event_Item_Target_Confirm);
		break;
		
	default:
		break;
	}
}

void UItemActionComponent::CancelCurrentItemAction()
{
	LOG_JJH(Warning, TEXT("[아이템 액션] 취소 : %s"), *ActiveItemID.ToString());

	if (CurrentActionMode == EItemActionMode::Targeting || CurrentActionMode == EItemActionMode::Range)
	{
		Server_SendInputEvent(DDGameplayTags::Event_Item_Target_Cancel);
	}
	
	// 아이템 수 원상복구
	RestoreCanceledItem(ActiveItemID);
	// 다시 인벤토리 창 띄우기
	if (const ADDBoardGameCharacter* OwnerCharacter = Cast<ADDBoardGameCharacter>(GetOwner()))
	{
		if (AController* OwnerController = OwnerCharacter->GetController())
		{
			ADDBasePlayerController* PC = Cast<ADDBasePlayerController>(OwnerController);
			PC->Client_OpenPopUp(DDGameplayTags::BoardGame_UI_Inventory); 
		}
	}
	ResetItemAction();
}

void UItemActionComponent::Server_ActivateItemAbility_Implementation(FName ItemID, FGameplayTag ItemAbilityTag,
                                                                     AActor* TargetActor)
{
	LOG_JJH(Warning, TEXT("[아이템 액션] 아이템 Ability 실행 요청 : %s, Target: %s"), *ItemID.ToString(),
	        *GetNameSafe(TargetActor));

	// 아이템 실행
	if (!TryActivateItemAbility(ItemID, ItemAbilityTag, TargetActor))
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

	Server_SendInputEvent(EventTag);

	if (Input == EItemTargetingInput::Confirm)
	{
		ResetItemAction();
	}
}

void UItemActionComponent::Server_SendInputEvent_Implementation(FGameplayTag EventTag)
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

	if (EventTag == DDGameplayTags::Event_Item_Target_Confirm ||
		EventTag == DDGameplayTags::Event_Item_Target_Cancel)
	{
		ResetItemAction();
	}
}

bool UItemActionComponent::TryActivateItemAbility(FName ItemID, FGameplayTag ItemAbilityTag, AActor* TargetActor)
{
	if (!ItemAbilityTag.IsValid())
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] ItemAbilityTag가 유효하지 않습니다. ItemID: %s"), *ItemID.ToString());
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

	FGameplayAbilitySpec* ItemAbilitySpec = nullptr;
	for (FGameplayAbilitySpec& AbilitySpec : AbilitySystemComp->GetActivatableAbilities())
	{
		if (AbilitySpec.Ability && AbilitySpec.Ability->GetAssetTags().HasTagExact(ItemAbilityTag))
		{
			ItemAbilitySpec = &AbilitySpec;
			break;
		}
	}

	if (!ItemAbilitySpec)
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] ItemAbilityTag에 맞는 Ability가 ASC에 부여되어 있지 않습니다. AbilitySet 등록 필요. ItemID: %s, Tag: %s"),
		        *ItemID.ToString(),
		        *ItemAbilityTag.ToString());
		return false;
	}
	
	UItemPayloadObject* PayloadObj = NewObject<UItemPayloadObject>(this);
	PayloadObj->ItemRow = ItemRowData;

	// 아이템 확성화 이벤트 태그를 발송
	FGameplayEventData EventData;
	EventData.EventTag = DDGameplayTags::Event_Item_Activate;
	EventData.Instigator = OwnerCharacter;
	EventData.Target = TargetActor;
	EventData.OptionalObject = PayloadObj;

	const bool bTriggered = AbilitySystemComp->TriggerAbilityFromGameplayEvent(
		ItemAbilitySpec->Handle,
		AbilitySystemComp->AbilityActorInfo.Get(),
		DDGameplayTags::Event_Item_Activate,
		&EventData,
		*AbilitySystemComp
	);
	if (!bTriggered)
	{
		LOG_JJH(Warning, TEXT("[아이템 액션] Ability 이벤트 트리거 실패. ItemID: %s, Tag: %s"),
		        *ItemID.ToString(),
		        *ItemAbilityTag.ToString());
		return false;
	}
	
	LOG_JJH(Warning, TEXT("[아이템 액션] Ability 실행 성공. ItemID: %s, Tag: %s, Target: %s"),
	        *ItemID.ToString(),
	        *ItemAbilityTag.ToString(),
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
	InventoryComponent->Server_AddItem(ItemID);
	LOG_JJH(Warning, TEXT("[아이템 액션] 취소 아이템 복구 : %s"), *ItemID.ToString());
}

void UItemActionComponent::ResetItemAction()
{
	RemoveItemActionTag();
	
	CurrentActionMode = EItemActionMode::None;
	ActiveItemID = NAME_None;
	ActiveItemTag = FGameplayTag();
	ActiveItemAbilityTag = FGameplayTag();
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
