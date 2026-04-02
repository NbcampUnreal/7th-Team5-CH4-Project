#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "DDBasePlayerState.generated.h"

class UDDPointSet;
class UDDAbilitySystemComponent;
class UDDHealthSet;
class UDDMovementSet;
// class ADDTile; 

UCLASS()
class DOODOONG_API ADDBasePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADDBasePlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FORCEINLINE UDDHealthSet* GetHealthSet() const { return HealthSet; }
	
	FORCEINLINE UDDPointSet* GetPointSet() const { return PointSet; }

	FORCEINLINE UDDMovementSet* GetMovementSet() const { return MovementSet; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = true))
	TObjectPtr<UDDAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UDDHealthSet> HealthSet;
	
	UPROPERTY()
	TObjectPtr<UDDPointSet> PointSet; 
	
	UPROPERTY()
	TObjectPtr<UDDMovementSet> MovementSet;
	
public:
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// ADDTile* CurrentTile;
};
