#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_HealingKit.generated.h"

UCLASS()
class DOODOONG_API UGA_HealingKit : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_HealingKit();

	virtual void ActivateAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealingKit")
	TSubclassOf<UGameplayEffect> HealingEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealingKit")
	float HealAmount = 20.0f;
};
