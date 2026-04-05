#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Attack.generated.h"


class UCharacterMovementComponent;

UCLASS()
class DOODOONG_API UGA_Attack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Attack(); 
	
	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, 
		const FGameplayTagContainer* TargetTags = nullptr, 
		FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled
	) override;
	
protected:
	UFUNCTION()
	void OnMontageCompleted(); 
	
	UFUNCTION()
	void OnReceiveTraceStart();

	UFUNCTION()
	void OnReceiveTraceEnd();
	
	void PerformTrace();
	
	void ApplyEffectsToTarget(AActor* TargetActor); 
	
	void LaunchTarget(AActor* TargetActor, float Strength);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* AttackMontage; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TArray<TSubclassOf<UGameplayEffect>> AdditionalEffectClasses;  
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float DamageAmount = 5.f; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float KnockBackStrength = 500.f; 
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace")
	FName TraceStartBone; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace")
	float TraceRadius; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace | Debug")
	bool bShowDebugTrace = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace | Debug")
	float DebugDuration; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace | Debug")
	FColor TraceColor;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace | Debug")
	FColor HitColor; 
	
private:
	ACharacter* CachedCharacter; 
	
	UAbilitySystemComponent* CachedAbilitySystemComponent;
	
	TSet<AActor*> HitActors; 
	
	FTimerHandle TraceTimerHandle; 
};
