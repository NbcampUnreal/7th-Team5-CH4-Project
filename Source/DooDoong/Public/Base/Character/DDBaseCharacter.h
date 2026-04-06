#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Data/DDAbilitySet.h"
#include "GameplayEffectTypes.h"
#include "DDBaseCharacter.generated.h"

class UDDAbilitySystemComponent;
class UDDHealthSet;
class UDDMovementSet;
struct FOnAttributeChangeData; 

UCLASS()
class DOODOONG_API ADDBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADDBaseCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

protected:
	void InitializeAbilitySystem();
	
	void BindAttributeDelegates();
	
	void OnWalkSpeedChanged(const FOnAttributeChangeData& Data);

	void OnJumpSpeedChanged(const FOnAttributeChangeData& Data);
	
public:
	UDDHealthSet* GetHealthSet() const; 

	UDDMovementSet* GetMovementSet() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDDAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | Default", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDDAbilitySet> DefaultAbilitySet;

	UPROPERTY()
	TObjectPtr<UDDHealthSet> HealthSet;

	UPROPERTY()
	TObjectPtr<UDDMovementSet> MovementSet;
};
