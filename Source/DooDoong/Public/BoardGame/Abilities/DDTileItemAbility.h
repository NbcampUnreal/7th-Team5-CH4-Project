#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DDTileItemAbility.generated.h"


UCLASS()
class DOODOONG_API UDDTileItemAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDDTileItemAbility();
	virtual void ActivateAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	
	UFUNCTION()
	void OnMontageCompleted();
	
	UFUNCTION()
	void OnMontageInterrupted();
	
	UPROPERTY(EditDefaultsOnly, Category = "UX")
	UAnimMontage* MontageToPlay;
	UPROPERTY(EditDefaultsOnly, Category = "UX")
	AActor* ItemActor;
};
