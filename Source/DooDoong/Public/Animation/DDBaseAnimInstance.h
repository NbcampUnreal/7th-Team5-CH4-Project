#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DDBaseAnimInstance.generated.h"

class UCharacterMovementComponent; 

UCLASS()
class DOODOONG_API UDDBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference")
	TObjectPtr<ACharacter> CachedCharacter;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference")
	TObjectPtr<UCharacterMovementComponent> CachedMovementComponent;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movememt | Variables")
	bool bIsFalling = false; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movememt | Variables")
	bool bShouldMove = false; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movememt | Variables")
	FVector Velocity; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movememt | Variables")
	float GroundSpeed; 
	
	
};
