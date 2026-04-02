// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameStateBase.h"
#include "DDPlatformerGameStateBase.generated.h"

class DDPlatformerPlayerController;

/**
 * 
 */
UCLASS()
class DOODOONG_API ADDPlatformerGameStateBase : public ADDMiniGameStateBase
{
	GENERATED_BODY()
public:
	ADDPlatformerGameStateBase();
public:
	virtual void BeginPlay() override;
	
public:
	/* 플레이어 게임 입장시 10초간 대기 */
	void WaitingGame(APlayerController* LoginPlayer);
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Platformer")
	float WaitingTime;
	
	FTimerHandle WaitingTimerHandle;
	
};
