#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "DDPlatformerGameMode.generated.h"

class ADDPlatformerGameState;
class ADDBasePlayerState;
class ADDBasePlayerController;
class ADDBaseCharacter;
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
	float PlayerMaxDistance;
	
	UPROPERTY()
	int32 PlayerRank;
	
	UPROPERTY()
	bool bIsGoalIn = false;
	
};

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
	
	void GetPlayerSlotIndex();
	
	/*대기시간 타이머 호출 함수*/
	void WaitingTimerStart();
	
	/* 플레이어 이동거리 체크 타이머 함수 */
	void PlayGameTimer();
	
	void CheckGoalPlayerCharacter(AActor* GoalActor);
	
	void PlayerRanking();
	
	/* 대기시간 타이머 종료 게임시작 */
	void GameStart();
	
	void GameEnd();
	
	/* 구조체 정보전달 함수 */
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* PlatformerEnteredPlayer) override;
	
public:
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	TMap<int32, FPlatformerPlayerData> PlayerDatas;
	
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
	TArray<TWeakObjectPtr<ADDBasePlayerController>> PlayerRankingArrays;
	
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	TArray<ADDBaseCharacter*> PlayerGoalInArrays;
	
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	TArray<float> PlayerMaxDistances;
	
	/*UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	TMap<FName, ADDBasePlayerController*> PlayerDatas;*/
	
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
	int32 Rank = 1;
	int32 MaxPlayer = 4;
	
	/* 테스트용 임시변수 */
	int32 PlayerIndex = 1;
};
