#pragma once
#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DDBaseAttributeSet.generated.h"

UCLASS()
class DOODOONG_API UDDBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UDDBaseAttributeSet();

	ATTRIBUTE_ACCESSORS_BASIC(UDDBaseAttributeSet, Health); 
	ATTRIBUTE_ACCESSORS_BASIC(UDDBaseAttributeSet, MaxHealth); 
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

public:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	
private:
	UPROPERTY(BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = true), ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health; 
	
	UPROPERTY(BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = true), ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth; 
};
