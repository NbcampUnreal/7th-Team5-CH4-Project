// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DDMiniGameManager.generated.h"

/**
 * 
 */
UCLASS()
class DOODOONG_API UDDMiniGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	/** 서브시스템 생명주기 관련 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};
