#pragma once
#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DDHealthSet.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FKillLogSignature, AActor* /*Killer*/, AActor* /*Victim*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, int32 /*Cur*/, int32 /*Max*/);

UCLASS()
class DOODOONG_API UDDHealthSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UDDHealthSet();

	ATTRIBUTE_ACCESSORS_BASIC(UDDHealthSet, Health); 
	ATTRIBUTE_ACCESSORS_BASIC(UDDHealthSet, MaxHealth); 
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
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
	
public:
	FKillLogSignature KillLogSignature;
	
	FOnHealthChanged OnHealthChanged;
};
