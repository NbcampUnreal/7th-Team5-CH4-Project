#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Data/DDItemDataTypes.h"
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

	virtual void EndAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Cue")
	FGameplayTag ConfirmCueTag;

	ADDBoardGameCharacter* GetBoardGameCharacter() const;

	/** 보드게임 캐릭터가 사용하는 ASC Getter */
	UAbilitySystemComponent* GetBoardGameAbilitySystemComponent() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "UX")
	AActor* ItemActor;
	
	UPROPERTY()
	FItemTableRow CachedItemData;

	bool ExecuteItemCue(const FGameplayTag& CueTag) const;
	bool ExecuteItemCueOnTarget(const FGameplayTag& CueTag, AActor* CueActor) const;
};
