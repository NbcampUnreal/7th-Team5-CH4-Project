#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DDTileDamageAbility.generated.h"

UCLASS()
class DOODOONG_API UDDTileDamageAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UDDTileDamageAbility();
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> GE_Damage;
	
	UPROPERTY(EditDefaultsOnly, Category = "UX")
	FGameplayTag TileEventTag;
};
