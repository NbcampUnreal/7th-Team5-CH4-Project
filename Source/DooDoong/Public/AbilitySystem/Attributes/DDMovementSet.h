#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DDMovementSet.generated.h"

UCLASS()
class DOODOONG_API UDDMovementSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UDDMovementSet();
	
	ATTRIBUTE_ACCESSORS_BASIC(UDDMovementSet, MoveSpeed); 
	
	ATTRIBUTE_ACCESSORS_BASIC(UDDMovementSet, JumpSpeed); 
protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

public:
	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);
	
	UFUNCTION()
	void OnRep_JumpSpeed(const FGameplayAttributeData& OldJumpSpeed);
	
private:
	UPROPERTY(BlueprintReadOnly, Category = "Attribute", meta = (AllowPrivateAccess = true), ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed; 

	UPROPERTY(BlueprintReadOnly, Category = "Attribute", meta = (AllowPrivateAccess = true), ReplicatedUsing = OnRep_JumpSpeed)
	FGameplayAttributeData JumpSpeed; 
};

