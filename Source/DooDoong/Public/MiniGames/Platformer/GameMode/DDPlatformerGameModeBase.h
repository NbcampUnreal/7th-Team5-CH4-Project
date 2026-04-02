// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "DDPlatformerGameModeBase.generated.h"

class ADDPlatformerPlayerController;

/**
 * 
 */
UCLASS()
class DOODOONG_API ADDPlatformerGameModeBase : public ADDMiniGameModeBase
{
	GENERATED_BODY()
public:
	virtual void OnPostLogin(AController* NewPlayer) override;
	
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "PlatformerData")
	TObjectPtr<UDDInputConfig> PlatformerInputConfig;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PlatformerIMC;
	
private:
	TArray<ADDBasePlayerController*> AllPlayerControllers;
	
	int32 MaxPlayerControllers;
};
