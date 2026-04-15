#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DDTileGoalAbility.generated.h"

UCLASS()
class DOODOONG_API UDDTileGoalAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDDTileGoalAbility();
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	
	UFUNCTION()
	void OnMontageCompleted();
	
	UFUNCTION()
	void OnMontageInterrupted();

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> GE_Trophy;

	UPROPERTY(EditDefaultsOnly, Category = "UX")
	UAnimMontage* MontageToPlay;
};
