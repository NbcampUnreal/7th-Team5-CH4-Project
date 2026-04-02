// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "DDPlatformerGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DOODOONG_API ADDPlatformerGameModeBase : public ADDMiniGameModeBase
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

};
