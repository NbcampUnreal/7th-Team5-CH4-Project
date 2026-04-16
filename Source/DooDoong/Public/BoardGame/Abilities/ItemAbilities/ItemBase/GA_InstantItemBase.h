#pragma once

#include "CoreMinimal.h"
#include "BoardGame/Abilities/ItemAbilities/ItemBase/GA_ItemBase.h"
#include "GA_InstantItemBase.generated.h"

UCLASS(Abstract)
class DOODOONG_API UGA_InstantItemBase : public UGA_ItemBase
{
	GENERATED_BODY()

public:
	/** 즉시 사용 아이템의 공통적인 실행 흐름 */
	virtual void ActivateAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	/** 실제 즉시 아이템 효과 구현 지점 */
	virtual bool ExecuteInstantItem(const FGameplayEventData* TriggerEventData);
};
