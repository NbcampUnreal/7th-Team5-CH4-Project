#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Data/DDItemDataTypes.h"
#include "GA_Range.generated.h"

UCLASS()
class DOODOONG_API UGA_Range : public UGameplayAbility
{
	GENERATED_BODY()

public:
	void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	void ActivateAbilityFromEvent(const FGameplayEventData& EventData);
	
	UFUNCTION()
	void UpdateTargets();

	UFUNCTION()
	bool IsInFanRange(
		const FVector& Origin,
		const FVector& Forward,
		const FVector& TargetLoc);

	FItemTableRow CachedItemData;

	UPROPERTY(EditAnywhere)
	float Radius = 500.f;

	UPROPERTY(EditAnywhere)
	float AngleDeg = 90.f;

	TArray<AActor*> CachedTargets;
	FTimerHandle UpdateTimer;
};
