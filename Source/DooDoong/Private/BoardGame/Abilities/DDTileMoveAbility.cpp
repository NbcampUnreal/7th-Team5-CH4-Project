#include "BoardGame/Abilities/DDTileMoveAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/DDTile.h"
#include "BoardGame/Abilities/DDMoveTileStepTask.h"
#include "Common/DDLogManager.h"
#include "System/DDGameplayTags.h"

UDDTileMoveAbility::UDDTileMoveAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UDDTileMoveAbility::ActivateAbility(
	FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!HasAuthority(&ActivationInfo))
	{
		return; // 서버 실행
	}
	LOG_CYS(Warning, TEXT("Move Tile Event!"));
	
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(DDGameplayTags::State_TurnPhase_Moving);
	
	// 플레이어 스테이트 
	ADDBasePlayerState* PS = Cast<ADDBasePlayerState>(ActorInfo->OwnerActor.Get());
	if (!PS) return;
	
	ADDTile* Tile = PS->CurrentTile;
	if (!Tile) return;

	// 타일 정보 가져옴 
	AddMove = Tile->TileData.MoveEffect;
	
	// UX 지시
	ADDBoardGameCharacter* Character = Cast<ADDBoardGameCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return;
	
	Character->Multicast_ShowTileContentAboveHead(TileEventTag);
	
	// 딜레이
	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, 2.0f);
	DelayTask->OnFinish.AddDynamic(this, &UDDTileMoveAbility::OnMove);
	DelayTask->ReadyForActivation();
}

void UDDTileMoveAbility::OnMove()
{
	// 이동 태스크
	UDDMoveTileStepTask* Task = UDDMoveTileStepTask::MoveTile(this, AddMove);
	if (!Task)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	Task->OnFinished.AddUniqueDynamic(this, &UDDTileMoveAbility::OnMoveFinished);
	Task->ReadyForActivation();
}
void UDDTileMoveAbility::OnMoveFinished()
{
	LOG_CYS(Warning, TEXT("[GA_TileMove] Move Finished"));

	// 타일 Ability 트리거 또 실행할건가? 일단 NO.
	// ADDBoardGameCharacter* Character =
	// 	Cast<ADDBoardGameCharacter>(GetAvatarActorFromActorInfo());
	// if (!Character) return;
	//
	// ADDBasePlayerState* PS = Character->GetPlayerState<ADDBasePlayerState>();
	// if (!PS) return;
	//
	// ADDTile* CurrentTile = PS->CurrentTile;
	// if (CurrentTile)
	// {
	// 	CurrentTile->OnCharacterArrived(Character);
	// }
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(DDGameplayTags::State_TurnPhase_Moving);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
