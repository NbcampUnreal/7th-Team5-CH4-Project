#include "BoardGame/Abilities/DDTileItemAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Base/Player/DDBasePlayerController.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "Common/DDLogManager.h"
#include "Data/DDItemDataTypes.h"
#include "UI/Inventory/DDInventoryComponent.h"

UDDTileItemAbility::UDDTileItemAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UDDTileItemAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (HasAuthority(&ActivationInfo))
	{
		LOG_CYS(Warning, TEXT("Item Tile Event!"));
		ADDBoardGameCharacter* Character = Cast<ADDBoardGameCharacter>(GetAvatarActorFromActorInfo());
		if (!Character)
		{
			LOG_CYS(Warning, TEXT("Item Tile : 캐릭터 없다"));
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		ADDBasePlayerState* PS = Cast<ADDBasePlayerState>(ActorInfo->OwnerActor.Get());
		if (!PS) 
		{
			LOG_CYS(Warning, TEXT("Item Tile : PS 없다"));
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		UDDInventoryComponent* Inventory = PS->FindComponentByClass<UDDInventoryComponent>();
		if (!Inventory)
		{
			LOG_CYS(Warning, TEXT("Item Tile : 인벤 없다"));
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		FName ItemName = Inventory->AddRandomItem();
		if (ItemName.IsNone())
		{
			LOG_CYS(Warning, TEXT("Item Tile : 랜덤 아이템 없다"));
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}
		LOG_CYS(Warning, TEXT("랜덤 아이템 획득: %s"), *ItemName.ToString());

		FItemTableRow* Row = Inventory->GetItemData(ItemName);
		if (!Row || !Row->ItemActorClass)
		{
			LOG_CYS(Warning, TEXT("Item Tile : 아이템 정보 없다"));
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		// 손에 들기
		ItemActor = GetWorld()->SpawnActor<AActor>(Row->ItemActorClass);

		// 클라 복제
		ItemActor->SetReplicates(true);
		
		ItemActor->AttachToComponent(
			Character->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("RightHand")
		);
	}
	
	// 딜레이 태스크
	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, 2.5f); // 이펙트 길이
	DelayTask->OnFinish.AddDynamic(this, &UDDTileItemAbility::OnCueFinished);
	DelayTask->ReadyForActivation();
}

void UDDTileItemAbility::OnCueFinished()
{
	// 손에 액터 제거
	if (HasAuthority(&CurrentActivationInfo))
	{
		if (IsValid(ItemActor))
		{
			ItemActor->Destroy();
		}
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
