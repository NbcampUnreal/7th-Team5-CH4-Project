#pragma once

#include "CoreMinimal.h"
#include "GA_ItemBase.h"
#include "GA_RangeItemBase.generated.h"

UCLASS()
class DOODOONG_API UGA_RangeItemBase : public UGA_ItemBase
{
	GENERATED_BODY()

public:
	UGA_RangeItemBase();
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

protected:
	UFUNCTION()
	void UpdateDebug();

	void FindTargets();
	UFUNCTION()
	bool IsInFanRange(
		const FVector& Origin,
		const FVector& Forward,
		const FVector& TargetLoc);

	/** 현재 선택된 타겟으로 아이템 효과를 확정 */
	UFUNCTION()
	virtual void OnConfirm(FGameplayEventData Payload);

	/** 타게팅 Ability를 취소 종료 -> 아이템 수 복구는 ItemActionComponent 취소에서 함 */
	UFUNCTION()
	void OnCancel(FGameplayEventData Payload);

	UPROPERTY(EditAnywhere)
	float Radius = 1000.f;

	UPROPERTY(EditAnywhere)
	float AngleDeg = 60.f;

	TArray<ADDBoardGameCharacter*> CachedTargets;
	FTimerHandle UpdateTimer;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	FGameplayTag EffectDataTag;
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	float EffectValue = 0;

	/** 타겟에게 재생할 GameplayCue 태그 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Cue")
	FGameplayTag TargetCueTag;

	float TotalDrainedAmount = 0;
};
