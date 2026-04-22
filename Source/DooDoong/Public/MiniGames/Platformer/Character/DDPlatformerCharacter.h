// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/Character/DDBaseCharacter.h"
#include "DDPlatformerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class DOODOONG_API ADDPlatformerCharacter : public ADDBaseCharacter
{
	GENERATED_BODY()

public:
	ADDPlatformerCharacter();

protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(Server, Reliable)
	void Server_ReturnToSavePoint();
	
	UFUNCTION(Server, Reliable)
	void Server_SetSavePointLocation(const FVector& UpDateSavePoint);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TObjectPtr<UCameraComponent> CameraComp;
	
public:
	UPROPERTY()
	float PlayerMaxDistance = 0.f;
	
	UPROPERTY()
	int32 PlayerRank = -1;
	
	UPROPERTY()
	bool bIsGoalIn = false;
private:
	
	
	UPROPERTY()
	FVector SavePointLocation = FVector(0, 0, 0);
};
