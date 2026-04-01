// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/Game/MiniGame/MiniGameStateBase.h"
#include "DDPlatformerGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class DOODOONG_API ADDPlatformerGameStateBase : public AMiniGameStateBase
{
	GENERATED_BODY()
public:
	ADDPlatformerGameStateBase();
public:
	virtual void BeginPlay() override;
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Platformer")
	float WatingTime;
};
