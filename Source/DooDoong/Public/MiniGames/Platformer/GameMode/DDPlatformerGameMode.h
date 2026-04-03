// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "DDPlatformerGameMode.generated.h"

class ADDPlatformerPlayerController;
class ADDPlatformerGameState;
class ADDBasePlayerController;
class ADDBaseCharacter;
class UInputMappingContext;
class UDDInputConfig;

/**
 * 
 */
UCLASS()
class DOODOONG_API ADDPlatformerGameMode : public ADDMiniGameModeBase
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
	void PlayeGameTimer();
	
	void CheckGoalPlayerCharacter(AActor* GoalActor);
	
	void PlayerRanking();
	
	/* 대기시간 타이머 종료 게임시작 */
	void GameStart();
	
	void GameEnd();
	
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "PlatformerData")
	TObjectPtr<UDDInputConfig> PlatformerInputConfig;
	
	UPROPERTY(EditDefaultsOnly, Category = "PlatformerData")
	TObjectPtr<UInputMappingContext> PlatformerIMC;
	
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	TArray<ADDBasePlayerController*> AllPlayerControllers;
	
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	TArray<ADDBaseCharacter*> AllPlayerCharacters;
	
	UPROPERTY(EditAnywhere, Category = "PlatformerData")
	float PlatformerPlayTime = 30.f;
	
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	TArray<ADDBaseCharacter*> PlayerRankingArrays;
	
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	TArray<ADDBaseCharacter*> PlayerGoalInArrays;
	
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	TArray<float> PlayerMaxDistances;
	
	/* 게임스테이트 변수 */
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	ADDPlatformerGameState* PlatformerGameStateBase;
	
private:
	/* 타이머핸들러 */
	FTimerHandle FinishedWaitingTimerHandle;
	FTimerHandle DistanceTimerHandle;
	FTimerHandle PlatformerPlayTimerHandle;
	
	/* 게임중인 플레이어 관련 변수 */
	FVector StartLocation;
	
	/* 게임플레이 변수 */
	int32 GamePlayTime = 0;
};
