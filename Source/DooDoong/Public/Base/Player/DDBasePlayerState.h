#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "DDBasePlayerState.generated.h"

class UDDPointSet;
class UDDAbilitySystemComponent;
class UDDHealthSet;
class UDDMovementSet;

UCLASS()
class DOODOONG_API ADDBasePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADDBasePlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UDDHealthSet* GetHealthSet() const { return HealthSet; }
	
	UDDPointSet* GetPointSet() const { return PointSet; }

	UDDMovementSet* GetMovementSet() const { return MovementSet; }
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UDDAbilitySystemComponent> AbilitySystemComponent;

private:
	UPROPERTY()
	TObjectPtr<UDDHealthSet> HealthSet;
	
	UPROPERTY()
	TObjectPtr<UDDPointSet> PointSet; 
	
	UPROPERTY()
	TObjectPtr<UDDMovementSet> MovementSet;
;
};
