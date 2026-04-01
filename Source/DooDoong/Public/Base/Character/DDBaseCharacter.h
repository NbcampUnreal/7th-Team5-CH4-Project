

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "BoardGame/DDTile.h"
#include "DDBaseCharacter.generated.h"

class UDDAbilitySystemComponent;

UCLASS()
class DOODOONG_API ADDBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADDBaseCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void PossessedBy(AController* NewController) override;
	
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Dump:임시 상태 // 나중에 지울 것
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ADDTile* StartTile;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDDAbilitySystemComponent> AbilitySystemComponent;
	
};
