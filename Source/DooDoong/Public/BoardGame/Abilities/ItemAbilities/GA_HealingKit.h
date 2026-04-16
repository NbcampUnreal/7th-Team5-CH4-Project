#pragma once

#include "CoreMinimal.h"
#include "BoardGame/Abilities/ItemAbilities/ItemBase/GA_InstantItemBase.h"
#include "GA_HealingKit.generated.h"

UCLASS()
class DOODOONG_API UGA_HealingKit : public UGA_InstantItemBase
{
	GENERATED_BODY()

public:
	UGA_HealingKit();

protected:
	virtual bool ExecuteInstantItem(const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealingKit")
	TSubclassOf<UGameplayEffect> HealingEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealingKit")
	float HealAmount = 20.0f;
};
