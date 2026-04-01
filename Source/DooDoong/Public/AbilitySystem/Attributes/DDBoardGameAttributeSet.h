#pragma once
#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DDBoardGameAttributeSet.generated.h"

UCLASS()
class DOODOONG_API UDDBoardGameAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UDDBoardGameAttributeSet();

	ATTRIBUTE_ACCESSORS_BASIC(UDDBoardGameAttributeSet, Health); 
	ATTRIBUTE_ACCESSORS_BASIC(UDDBoardGameAttributeSet, MaxHealth); 
	ATTRIBUTE_ACCESSORS_BASIC(UDDBoardGameAttributeSet, Coin); 
	ATTRIBUTE_ACCESSORS_BASIC(UDDBoardGameAttributeSet, Trophy); 
	
protected:
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health; 
	
	UPROPERTY(BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth; 
	
	UPROPERTY(BlueprintReadOnly, Category = "Point", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Coin;
	
	UPROPERTY(BlueprintReadOnly, Category = "Point", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Trophy; 
	
};
