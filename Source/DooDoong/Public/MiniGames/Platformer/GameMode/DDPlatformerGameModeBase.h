// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "DDPlatformerGameModeBase.generated.h"

class ADDPlatformerPlayerController;
class ADDPlatformerGameStateBase;
class ADDBasePlayerController;
class ADDBaseCharacter;
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
	
	/* 플레이어 이동거리 체크 타이머 함수 */
	void CheckPlayerMovedDistanceTimer();
	
	/* 대기시간 타이머 종료 게임시작 */
	void GameStart();
	
	void GameEnd();
	
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "PlatformerData")
	TObjectPtr<UDDInputConfig> DDPlatformerInputConfig;
	
	UPROPERTY(EditDefaultsOnly, Category = "PlatformerData")
	TObjectPtr<UInputMappingContext> DDPlatformerIMC;
	
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	TArray<ADDBasePlayerController*> DDAllPlayerControllers;
	
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	TArray<ADDBaseCharacter*> DDAllPlayerCharacters;
	
	UPROPERTY(EditAnywhere, Category = "PlatformerData")
	float PlatformerPlayTime = 30.f;
	
private:
	/* 타이머핸들러 */
	FTimerHandle FinishedWaitingTimerHandle;
	FTimerHandle CheckPlayerMovedDistanceTimerHandle;
	FTimerHandle PlatformerPlayTimerHandle;
	
	/* 게임스테이트 변수 */
	ADDPlatformerGameStateBase* PlatformerGameStateBase;
	
	/* 게임중인 플레이어 관련 변수 */
	FVector StartLocation;
	
	TArray<float> DDPlayerMaxDistance;
};
