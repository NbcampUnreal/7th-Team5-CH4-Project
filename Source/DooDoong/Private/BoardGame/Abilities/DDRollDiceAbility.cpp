#include "BoardGame/Abilities/DDRollDiceAbility.h"

#include "BoardGame/Abilities/DDMoveTileStep.h"
#include "Common/DDLogManager.h"

void UDDRollDiceAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthority(&ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	LOG_CYS(Warning,TEXT("[GA_RD]Roll Dice"));
	// 주사위
	int32 Dice = FMath::RandRange(1, 6);

	// 이동 Task 실행
	auto Task = UDDMoveTileStep::MoveTile(this, Dice);

	Task->OnFinished.AddDynamic(this, &UDDRollDiceAbility::K2_EndAbility);

	Task->ReadyForActivation();
}
