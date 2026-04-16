#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Targeting.generated.h"

class UItemActionComponent;

UCLASS()
class DOODOONG_API UGA_Targeting : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Targeting();

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

	void BuildTargetCandidates();
	void ChangeTarget(int32 Offset);
	void FocusSelectedTarget();
	void SendTargetResultEvent(const FGameplayTag& EventTag, AActor* TargetActor);
	AActor* GetSelectedTarget() const;

protected:
	UPROPERTY()
	TObjectPtr<UItemActionComponent> ItemActionComponent;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> CandidateTargets;

	UPROPERTY()
	int32 SelectedTargetIndex = INDEX_NONE;
};
