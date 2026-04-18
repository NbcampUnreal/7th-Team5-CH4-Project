#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GA_ItemBase.generated.h"

class ADDBoardGameCharacter;
class UAbilitySystemComponent;

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Cue")
	FGameplayTag ConfirmCueTag;

	ADDBoardGameCharacter* GetBoardGameCharacter() const;

	UAbilitySystemComponent* GetBoardGameAbilitySystemComponent() const;

	bool ExecuteItemCue(const FGameplayTag& CueTag, const FVector& CueLocation) const;
	bool ExecuteItemCueOnTarget(const FGameplayTag& CueTag, AActor* CueActor) const;
};
