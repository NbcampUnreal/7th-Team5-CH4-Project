//GA_Throw.h
#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Throw.generated.h"

class ADDRicochetCharacter;
class UAnimMontage;

UCLASS()
class DOODOONG_API UGA_Throw : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Throw();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

protected:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnReceiveThrowEvent(FGameplayEventData Payload);

protected:
	/** 비어 있으면 Character의 ThrowMontage 사용 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throw")
	TObjectPtr<UAnimMontage> ThrowMontage = nullptr;

private:
	UPROPERTY()
	TObjectPtr<ADDRicochetCharacter> CachedCharacter = nullptr;
};