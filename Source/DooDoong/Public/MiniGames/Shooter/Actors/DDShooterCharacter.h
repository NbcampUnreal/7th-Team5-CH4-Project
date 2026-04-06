#pragma once

#include "CoreMinimal.h"
#include "Base/Character/DDBaseCharacter.h"
#include "DDShooterCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class DOODOONG_API ADDShooterCharacter : public ADDBaseCharacter
{
	GENERATED_BODY()

public:
	ADDShooterCharacter();
	
public:
	virtual void BeginPlay() override;
	
	bool CanFire() const;
	void TryFire();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<USceneComponent> MuzzleComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<UCameraComponent> CameraComp;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter")
	float AimTraceDistance = 5000.0f;
};
