#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DDRollDiceAbility.generated.h"


UCLASS()
class DOODOONG_API UDDRollDiceAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDDRollDiceAbility();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled
	) override;

protected:
	UFUNCTION()
	void OnDiceAnimationFinished();

	UFUNCTION()
	void OnMoveFinished();

private:
	int32 DiceResult = 0;
};
