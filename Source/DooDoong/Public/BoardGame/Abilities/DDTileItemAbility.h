#pragma once

#include "CoreMinimal.h"
#include "DDTileBaseAbility.h"
#include "Abilities/GameplayAbility.h"
#include "DDTileItemAbility.generated.h"


UCLASS()
class DOODOONG_API UDDTileItemAbility : public UDDTileBaseAbility
{
	GENERATED_BODY()

public:
	UDDTileItemAbility();
	virtual void ActivateAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "UX")
	AActor* ItemActor;
};
