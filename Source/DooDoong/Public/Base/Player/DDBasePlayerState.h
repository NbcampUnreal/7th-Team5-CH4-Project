#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Attributes/DDBaseAttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "DDBasePlayerState.generated.h"


class UDDAbilitySystemComponent;

UCLASS()
class DOODOONG_API ADDBasePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	
	ADDBasePlayerState(); 
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UAttributeSet* GetAttributeSet() const { return AttributeSet;}

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UDDAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UDDBaseAttributeSet> AttributeSet;
};
