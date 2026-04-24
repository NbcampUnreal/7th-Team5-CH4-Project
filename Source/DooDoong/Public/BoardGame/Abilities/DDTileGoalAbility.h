#pragma once

#include "CoreMinimal.h"
#include "DDTileBaseAbility.h"
#include "Abilities/GameplayAbility.h"
#include "DDTileGoalAbility.generated.h"

UCLASS()
class DOODOONG_API UDDTileGoalAbility : public UDDTileBaseAbility
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
	
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> GE_Trophy;
};
