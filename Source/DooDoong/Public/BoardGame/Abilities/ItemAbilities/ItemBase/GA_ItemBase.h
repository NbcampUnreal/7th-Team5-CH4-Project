#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ItemBase.generated.h"

class ADDBoardGameCharacter;
class UAbilitySystemComponent;
class UAbilityTask_PlayMontageAndWait;
class UAnimMontage;

UCLASS(Abstract)
class DOODOONG_API UGA_ItemBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_ItemBase();

	virtual void ActivateAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	/** 아이템을 사용하는 보드게임 캐릭터 Getter */
	ADDBoardGameCharacter* GetBoardGameCharacter() const;

	/** 보드게임 캐릭터가 사용하는 ASC Getter */
	UAbilitySystemComponent* GetBoardGameAbilitySystemComponent() const;

	bool PlayItemMontage(float PlayRate = 1.f, FName StartSection = NAME_None);

	UFUNCTION()
	virtual void OnItemMontageCompleted();

	UFUNCTION()
	virtual void OnItemMontageInterrupted();

	UFUNCTION()
	virtual void OnItemMontageCancelled();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Animation")
	TObjectPtr<UAnimMontage> ItemMontage;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ActiveItemMontageTask;
};
