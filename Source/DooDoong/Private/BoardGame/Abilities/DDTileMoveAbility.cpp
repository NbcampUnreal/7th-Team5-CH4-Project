#include "BoardGame/Abilities/DDTileMoveAbility.h"

#include "Common/DDLogManager.h"

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

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
