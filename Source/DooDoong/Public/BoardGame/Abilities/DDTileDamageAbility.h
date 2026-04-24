#pragma once

#include "CoreMinimal.h"
#include "DDTileBaseAbility.h"
#include "Abilities/GameplayAbility.h"
#include "DDTileDamageAbility.generated.h"

UCLASS()
class DOODOONG_API UDDTileDamageAbility : public UDDTileBaseAbility
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
