#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_GiveBomb.generated.h"

UCLASS()
class DOODOONG_API UGA_GiveBomb : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_GiveBomb();
	
	virtual void ActivateAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GiveBomb")
	TSubclassOf<UGameplayEffect> DamageEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GiveBomb")
	float Damage;
};
