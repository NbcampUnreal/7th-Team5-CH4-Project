#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DDTileBaseAbility.generated.h"


UCLASS()
class DOODOONG_API UDDTileBaseAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	
	void ExecuteItemCue() const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	FGameplayTag CueTag;
};
