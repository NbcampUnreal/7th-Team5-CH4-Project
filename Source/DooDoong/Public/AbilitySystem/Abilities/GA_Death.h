#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Death.generated.h"

UCLASS()
class DOODOONG_API UGA_Death : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Death();
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData
	) override;
	
public:
	UFUNCTION()
	void OnMontageFinished();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* DeathMontage;
	
	UPROPERTY(EditDefaultsOnly)
	float DeathLifeSpan = 5.f;
};
