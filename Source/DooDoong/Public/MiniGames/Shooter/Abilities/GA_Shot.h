#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Shot.generated.h" 

UCLASS()
class DOODOONG_API UGA_Shot : public UGameplayAbility
{
	GENERATED_BODY()
	
	/** 발사 Ability 진입점. 코스트/쿨다운 커밋 후 ShooterCharacter::TryFire를 호출한다. */
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
};
