#include "BoardGame/Abilities/ItemAbilities/GA_Portal.h"

#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/DDTile.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "BoardGame/Game/DDBoardGameMode.h"
#include "Common/DDLogManager.h"

bool UGA_Portal::ExecuteTargetingItem(AActor* TargetActor)
{
	ADDBoardGameCharacter* CasterCharacter = GetBoardGameCharacter();
	if (!IsValid(CasterCharacter)) return false;
	ADDBoardGameCharacter* TargetCharacter = Cast<ADDBoardGameCharacter>(TargetActor);
	if (!IsValid(TargetCharacter)) return false;
	
	// 캐스터 플레이어 state
	ADDBasePlayerState* CasterState = CasterCharacter->GetPlayerState<ADDBasePlayerState>();
	// 타겟 플레이어 state
	ADDBasePlayerState* TargetState = TargetCharacter->GetPlayerState<ADDBasePlayerState>();
	if (!CasterState)
	{
		LOG_CYS(Warning, TEXT("캐스터 PS 없음"));
		return false;
	}
	if (!TargetState)
	{
		LOG_CYS(Warning, TEXT("타겟 PS 없음"));
		return false;
	}
	// CurrentTile 값 swap
	if (!CasterState->CurrentTile) return false;
	if (!TargetState->CurrentTile) return false;
	
	auto TempTile = CasterState->CurrentTile;
	CasterState->CurrentTile = TargetState->CurrentTile;
	TargetState->CurrentTile = TempTile;
	
	// 위치 이동
	CasterCharacter->SetActorLocation(CasterState->CurrentTile->GetStandLocation(CasterCharacter));
	TargetCharacter->SetActorLocation(TargetState->CurrentTile->GetStandLocation(TargetCharacter));
	
	LOG_CYS(Warning,TEXT("포탈 아이템: 자리 교환 완료"));
	
	return true;
}

void UGA_Portal::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 주사위 생략하기
	if (ADDBoardGameMode* GM = Cast<ADDBoardGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->NotifyDiceRolled();
		GM->NotifyMovementFinished();
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

