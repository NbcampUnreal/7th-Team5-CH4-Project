#pragma once

#include "CoreMinimal.h"
#include "BoardGame/Abilities/ItemAbilities/ItemBase/GA_ItemBase.h"
#include "GA_InstantItemBase.generated.h"

UCLASS(Abstract)
class DOODOONG_API UGA_InstantItemBase : public UGA_ItemBase
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	virtual bool ExecuteInstantItem(const FGameplayEventData* TriggerEventData);
};
