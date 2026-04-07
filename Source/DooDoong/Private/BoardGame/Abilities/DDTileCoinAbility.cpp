#include "BoardGame/Abilities/DDTileCoinAbility.h"

#include "Common/DDLogManager.h"

UDDTileCoinAbility::UDDTileCoinAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UDDTileCoinAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!HasAuthority(&ActivationInfo))
	{
		return; // 서버 실행
	}
	LOG_CYS(Warning, TEXT("Coin Tile Event!"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
