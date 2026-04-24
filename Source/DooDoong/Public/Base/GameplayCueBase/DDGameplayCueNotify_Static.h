#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "DDGameplayCueNotify_Static.generated.h"

class UNiagaraSystem;
class UAnimMontage;

UCLASS(Blueprintable)
class DOODOONG_API UDDGameplayCueNotify_Static : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemCue|Animation")
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemCue|Animation")
	FName MontageStartSection = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemCue|Animation", meta = (ClampMin = "0.01"))
	float PlayRate = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemCue|Sound")
	FName SoundID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemCue|VFX")
	TObjectPtr<UNiagaraSystem> Effect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemCue|VFX")
	FVector EffectScale = FVector::OneVector;

private:
	void PlayAnimation(const FGameplayCueParameters& Parameters) const;
	void PlaySound(const UObject* WorldContextObject, const FGameplayCueParameters& Parameters) const;
	void SpawnEffect(const UObject* WorldContextObject, const FGameplayCueParameters& Parameters) const;
};
