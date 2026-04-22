#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DDPointSet.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCoinChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTrophyChanged, int32);

UCLASS()
class DOODOONG_API UDDPointSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UDDPointSet();

	ATTRIBUTE_ACCESSORS_BASIC(UDDPointSet, Trophy); 
	ATTRIBUTE_ACCESSORS_BASIC(UDDPointSet, Coin); 
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
public:
	UFUNCTION()
	void OnRep_Trophy(const FGameplayAttributeData& OldTrophy);

	UFUNCTION()
	void OnRep_Coin(const FGameplayAttributeData& OldCoin);
private:
	UPROPERTY(BlueprintReadOnly, Category = "Trophy", meta = (AllowPrivateAccess = true), ReplicatedUsing = OnRep_Trophy)
	FGameplayAttributeData Trophy; 
	
	UPROPERTY(BlueprintReadOnly, Category = "Coin", meta = (AllowPrivateAccess = true), ReplicatedUsing = OnRep_Coin)
	FGameplayAttributeData Coin; 
	
public:
	FOnCoinChanged OnCoinChanged;
	
	FOnTrophyChanged OnTrophyChanged;
	
	float LastCoinLose;
};
