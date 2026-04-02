#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Attributes/DDHealthSet.h"
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

	UDDHealthSet* GetHealthSet() const { return HealthSet; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UDDAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UDDHealthSet> HealthSet;
};
