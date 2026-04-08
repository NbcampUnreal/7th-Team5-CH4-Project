#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DDTileCoinAbility.generated.h"

UCLASS()
class DOODOONG_API UDDTileCoinAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDDTileCoinAbility();
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> GE_Coin;
	
	UPROPERTY(EditDefaultsOnly, Category = "UX")
	FGameplayTag TileEventTag;
};
