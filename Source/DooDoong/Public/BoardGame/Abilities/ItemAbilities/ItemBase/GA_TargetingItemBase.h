#pragma once

#include "CoreMinimal.h"
#include "BoardGame/Abilities/ItemAbilities/ItemBase/GA_ItemBase.h"
#include "GA_TargetingItemBase.generated.h"

UCLASS(Abstract)
class DOODOONG_API UGA_TargetingItemBase : public UGA_ItemBase
{
	GENERATED_BODY()

public:
	UGA_TargetingItemBase();

	virtual void ActivateAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnTargetNext(FGameplayEventData Payload);

	UFUNCTION()
	void OnTargetPrevious(FGameplayEventData Payload);

	UFUNCTION()
	void OnTargetConfirm(FGameplayEventData Payload);

	UFUNCTION()
	void OnTargetCancel(FGameplayEventData Payload);

	virtual bool ExecuteTargetingItem(AActor* TargetActor);

	void BuildTargetCandidates();
	void ChangeTarget(int32 Offset);
	void FocusSelectedTarget();
	AActor* GetSelectedTarget() const;

protected:
	UPROPERTY()
	TArray<TObjectPtr<AActor>> CandidateTargets;

	UPROPERTY()
	int32 SelectedTargetIndex = INDEX_NONE;
};
