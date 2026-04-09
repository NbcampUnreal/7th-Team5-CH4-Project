#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Shot.generated.h"

class ADDShooterCharacter;
class UAnimMontage;

UCLASS()
class DOODOONG_API UGA_Shot : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Shot();

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
	void OnReceiveFireEvent(FGameplayEventData Payload);

protected:
	/** Ability에서 명시적으로 몽타주를 지정하고 싶을 때만 사용한다. 비어 있으면 Character의 FireMontage를 사용한다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<UAnimMontage> FireMontage = nullptr;

private:
	UPROPERTY()
	TObjectPtr<ADDShooterCharacter> CachedShooterCharacter = nullptr;
};
