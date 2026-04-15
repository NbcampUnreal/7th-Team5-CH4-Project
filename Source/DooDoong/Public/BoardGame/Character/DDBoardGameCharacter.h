#pragma once

#include "CoreMinimal.h"
#include "Base/Character/DDBaseCharacter.h"
#include "DDBoardGameCharacter.generated.h"

class UDDInventoryComponent;
class UItemActionComponent;
class UDDMoveTileStepTask;
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

	UFUNCTION()
	void PlayDice(int32 DiceValue);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDiceAnimation();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowTileContentAboveHead(FGameplayTag TileTag);
	
	
	UPROPERTY()
	FTimerHandle MoveTimerHandle;
	
	UPROPERTY()
	FTimerHandle EventDestroyTimerHandle;
	
	UPROPERTY()
	FOnMoveFinished OnMoveFinished;

	UPROPERTY(EditAnywhere, Category="Dice")
	UAnimMontage* DiceMontage;
	
	UPROPERTY()
	UDDMoveTileStepTask* CurrentMoveTask;
	
	UPROPERTY(EditAnywhere, Category="Object")
	TSubclassOf<class AActor> SelectableActorClass;
	
protected:
	/** 주현 : ItemActionComp 추가 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Comp")
	TObjectPtr<UItemActionComponent> ItemActionComp;
	
private:
	FVector MoveStartLocation;
	FVector MoveTargetLocation;

	float MoveElapsedTime = 0.f;
	float MoveDuration = 0.3f;

	bool bIsMoving = false;
	
	UPROPERTY(EditAnywhere, Category="Object")
	TSubclassOf<class ADDDiceActor> DiceClass;
	
	UPROPERTY(EditAnywhere, Category="Object")
	TSubclassOf<class AActor> CoinClass;
	
	UPROPERTY(EditAnywhere, Category="Object")
	TSubclassOf<class AActor> MoveClass;
	
	UPROPERTY(EditAnywhere, Category="Object")
	TSubclassOf<class AActor> DamageClass;
	
	UPROPERTY()
	ADDDiceActor* Dice;
	UPROPERTY()
	AActor* EventActor;
	
	UPROPERTY(EditAnywhere, Category="UX")
	float HeadOffset = 70.f;
};
