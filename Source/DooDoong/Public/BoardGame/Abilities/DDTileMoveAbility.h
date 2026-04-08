#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DDTileMoveAbility.generated.h"

UCLASS()
class DOODOONG_API UDDTileMoveAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDDTileMoveAbility();
	virtual void ActivateAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	
	UFUNCTION()
	void OnMove();
	
	UPROPERTY()
	int32 AddMove;

	UFUNCTION()
	void OnMoveFinished();

	UPROPERTY(EditDefaultsOnly, Category = "UX")
	FGameplayTag TileEventTag;
};
