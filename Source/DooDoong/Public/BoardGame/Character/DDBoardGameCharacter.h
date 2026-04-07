#pragma once

#include "CoreMinimal.h"
#include "Base/Character/DDBaseCharacter.h"
#include "DDBoardGameCharacter.generated.h"

class UDDBoardGameAttributeSet;
class UDDHealthSet;
class ADDDiceActor;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveFinished);
 

UCLASS()
class DOODOONG_API ADDBoardGameCharacter : public ADDBaseCharacter
{
	GENERATED_BODY()
	
public:
	ADDBoardGameCharacter();
	
	UFUNCTION()
	void InitLocation();

	UFUNCTION(BlueprintCallable)
	void MoveToLocation(FVector TargetLocation);
	
	UFUNCTION(BlueprintCallable)
	void UpdateMove();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDiceAnimation(int32 DiceValue);
	
	UPROPERTY()
	FTimerHandle MoveTimerHandle;
	
	UPROPERTY()
	FOnMoveFinished OnMoveFinished;

	UPROPERTY(EditAnywhere, Category="Dice")
	UAnimMontage* DiceMontage;
private:
	FVector MoveStartLocation;
	FVector MoveTargetLocation;

	float MoveElapsedTime = 0.f;
	float MoveDuration = 0.3f;

	bool bIsMoving = false;
	
	UPROPERTY(EditAnywhere, Category="Dice")
	TSubclassOf<class ADDDiceActor> DiceClass;
	
	ADDDiceActor* Dice;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDDHealthSet> AttributeSet;
};
