#pragma once

#include "CoreMinimal.h"
#include "BoardGame/Abilities/ItemAbilities/ItemBase/GA_TargetingItemBase.h"
#include "GA_GiveBomb.generated.h"

UCLASS()
class DOODOONG_API UGA_GiveBomb : public UGA_TargetingItemBase
{
	GENERATED_BODY()
	
public:
	UGA_GiveBomb();
	
protected:
	virtual bool ExecuteTargetingItem(AActor* TargetActor) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GiveBomb")
	TSubclassOf<UGameplayEffect> DamageEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GiveBomb")
	float Damage = 20.f;
};
