#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "DDPlatformerGameMode.generated.h"

class ADDPlatformerCharacter;
class ADDPlatformerGameState;
class ADDBasePlayerState;
class ADDBasePlayerController;
class UInputMappingContext;
class UDDInputConfig;

USTRUCT(BlueprintType)
struct FPlatformerPlayerData
{
	GENERATED_BODY()
	/*BasePlayerState에서 넘겨받을 정보*/
	UPROPERTY()
	FLinearColor PlayerColor = FLinearColor::White; 

	UPROPERTY()
	FString PlayerDisplayName = TEXT("");
	
	UPROPERTY()
	int32 PlayerSlotIndex = 0;
	
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
	FVector SavePointLocation = FVector(0, 0, 0);
};

UCLASS()
class DOODOONG_API ADDPlatformerGameMode : public ADDMiniGameModeBase
{
	GENERATED_BODY()
public:
	/* 초기화 함수 */
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	
	virtual void BeginPlay() override;
	
	virtual void StartMiniGame() override;

public:
	/* 게임진행 관련 함수 */
	void SetEnteredPlayer();
	
	void CheckGoalPlayerCharacter(AActor* OverlapActor);
	
	void ReturnToSavePoint(AActor* OverlapActor);
	
	void UpdatePlayerSavePoint(AActor* OverlapActor, const FVector UpdateSavePoint);
	
	/* 플레이어 이동거리 체크 타이머 함수 */
	void PlayGameTimer();
	
	void PlayerRanking();
	
	/* 대기시간 타이머 종료 게임시작 */
	void GameStart();
	
	void GameEnd();
	
	void CallFinishGameTimer();
	
	
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "PlatformerData")
	TObjectPtr<UDDInputConfig> PlatformerInputConfig;
	
	UPROPERTY(EditDefaultsOnly, Category = "PlatformerData")
	TObjectPtr<UInputMappingContext> PlatformerIMC;
	/* 참가한 플레이어 관련 정보 */
	
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData | EnteredCharacter")
	TArray<TObjectPtr<ADDPlatformerCharacter>> EnteredPlayers;
	
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData | EnteredCharacter")
	TArray<TObjectPtr<ADDPlatformerCharacter>> NoGoalPlayers;
	
	/* 게임 진행시간 */
	UPROPERTY(EditAnywhere, Category = "PlatformerData")
	float PlatformerPlayTime = 120.f;
	
	/* 게임스테이트 변수 */
	UPROPERTY(VisibleAnywhere, Category = "PlatformerData")
	ADDPlatformerGameState* PlatformerGameStateBase;
	
private:
	/* 타이머핸들러 */
	FTimerHandle DistanceTimerHandle;
	FTimerHandle PlatformerPlayTimerHandle;
	FTimerHandle FinishGameTimerHandle;
	
	/* 게임중인 플레이어 관련 변수 */
	FVector2D StartLocation;
	int32 Rank = 1;
	
};
