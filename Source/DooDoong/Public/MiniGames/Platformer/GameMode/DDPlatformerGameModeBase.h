// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/Game/MiniGame/MiniGameModeBase.h"
#include "DDPlatformerGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DOODOONG_API ADDPlatformerGameModeBase : public AMiniGameModeBase
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

};
