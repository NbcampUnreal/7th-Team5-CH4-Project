#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "DDPlatformerGameMode.generated.h"

class ADDPlatformerGameState;
class ADDBasePlayerState;
class ADDBasePlayerController;
class UInputMappingContext;
class UDDInputConfig;

/**
 * 
 */

USTRUCT(BlueprintType)
struct FPlatformerPlayerData
{
	GENERATED_BODY()
	/*BasePlayerState에서 넘겨받을 정보*/
	UPROPERTY()
	FLinearColor PlayerColor; 

	UPROPERTY()
	FString PlayerDisplayName;
	
	UPROPERTY()
	int32 PlayerSlotIndex;
	
	/* 미니게임 에서 필요한 추가 정보 */
	UPROPERTY()
	TWeakObjectPtr<ADDBasePlayerController> PlayerController;
	
	UPROPERTY()
	TWeakObjectPtr<ADDBasePlayerState> PlayerState;
	
	UPROPERTY()
	float PlayerMaxDistance = 0.f;
	
	UPROPERTY()
	int32 PlayerRank = -1;
	
	UPROPERTY()
	bool bIsGoalIn = false;
	
	UPROPERTY()
	FVector SavePointLocation;

	// 추가: 플레이어 준비 상태
	UPROPERTY()
	bool bIsReady = false;
};

UCLASS()
class DOODOONG_API ADDPlatformerGameMode : public ADDMiniGameModeBase
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void StartMiniGame() override;
	
public:
	/*플레이어 준비완료 체크*/
	void CheckReadyPlayers();

	void SetPlayerReady(int32 SlotIndex);
	
	/*대기시간 타이머 호출 함수*/
	void WaitingTimerStart();
	
	/* 플레이어 이동거리 체크 타이머 함수 */
	void PlayGameTimer();
	
	void CheckGoalPlayerCharacter(AActor* GoalActor);
	
	void PlayerRanking();
	
	/* 대기시간 타이머 종료 게임시작 */
	void GameStart();
	
	void GameEnd();	
	
	/* 초기화 함수 */
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
public:
	/* 참가한 플레이어 관련 정보 */
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData | EnteredPlayer")
	TMap<int32, FPlatformerPlayerData> PlayerDatas;

	int32 GetReadyPlayerCount() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "PlatformerData")
	TObjectPtr<UDDInputConfig> PlatformerInputConfig;
	
	UPROPERTY(EditDefaultsOnly, Category = "PlatformerData")
	TObjectPtr<UInputMappingContext> PlatformerIMC;
	
	/* 참가한 플레이어 관련 정보 */
	
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData | EnteredPlayer")
	TMap<int32, FPlatformerPlayerData> PlayerRankingArrays;
	
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData | EnteredPlayer")
	TMap<int32, FPlatformerPlayerData> PlayerNoGoalArrays;
	
	/* 게임 진행시간 */
	UPROPERTY(EditAnywhere, Category = "PlatformerData")
	float PlatformerPlayTime = 30.f;
	
	/* 게임스테이트 변수 */
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	ADDPlatformerGameState* PlatformerGameStateBase;

	bool bReadyUIShown = false;
	
private:
	/* 타이머핸들러 */
	FTimerHandle FinishedWaitingTimerHandle;
	FTimerHandle DistanceTimerHandle;
	FTimerHandle PlatformerPlayTimerHandle;
	
	/* 게임중인 플레이어 관련 변수 */
	FVector StartLocation;
	int32 Rank = 1;
	int32 MaxPlayer = 2;
	
	/* 테스트용 임시변수 */
	int32 PlayerIndex = 1;
	
};
