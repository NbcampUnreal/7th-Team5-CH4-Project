// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "DDPlatformerGameModeBase.generated.h"

class ADDPlatformerPlayerController;
class ADDPlatformerGameStateBase;
class ADDBasePlayerController;
class UInputMappingContext;
class UDDInputConfig;

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
	
public:
	/*플레이어 준비완료 체크*/
	void CheckReadyPlayers();
	
	/*대기시간 타이머 호출 함수*/
	void WaitingTimerStart();
	
	/* 대기시간 타이머 종료 게임시작 */
	void FinishedWaitingTimer();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "PlatformerData")
	TObjectPtr<UDDInputConfig> PlatformerInputConfig;
	
	UPROPERTY(EditDefaultsOnly, Category = "PlatformerData")
	TObjectPtr<UInputMappingContext> PlatformerIMC;
	
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	TArray<ADDBasePlayerController*> AllPlayerControllers;
	
private:
	/* 타이머핸들러 */
	FTimerHandle FinishedWaitingTimerHandle;
	
	/* 게임스테이트 변수 */
	ADDPlatformerGameStateBase* PlatformerGameStateBase;
};
