#pragma once

#include "CoreMinimal.h"
#include "DDMiniGameStateBase.h"
#include "DDMiniGameTypes.h"
#include "GameFramework/GameModeBase.h"
#include "DDMiniGameModeBase.generated.h"

class UDDMiniGameRuleSet;
struct FTimerHandle;
/**
 * 
 */
UCLASS()
class DOODOONG_API ADDMiniGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ADDMiniGameModeBase();
	
	virtual void BeginPlay() override;
	
public:
	/** 미니게임 데이터 Initialize */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	virtual void InitializeMiniGame(const FMiniGameSetup& InSetup, const TArray<FMiniGameParticipantInfo>& InParticipants);
	
	/** 미니게임 시작 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	virtual void StartMiniGame();
	
	/** 일반적인 미니게임 종료 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	virtual void FinishMiniGame();
	
	/** Reason에 의해 강제로 미니게임을 종료해야 하는 경우 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	virtual void StopMiniGame(FGameplayTag Reason);
	
	/** 미니게임 종료 시점에서 결과(순위 등)를 만드는 구간 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	virtual FMiniGameResult BuildMiniGameResult() const;
	
public:
	/** 점수 처리 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="MiniGame")
	void AddScore(APlayerState* PlayerState, int32 DeltaScore);
	
	/** 실제 종료 처리 : Reason 태그에 의해 종료됨 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="MiniGame")
	void FinishGame(FGameplayTag Reason);

protected:
	/** MiniGameStateBase를 가져오는 헬퍼 */
	TObjectPtr<ADDMiniGameStateBase> GetMiniGameState() const { return GetGameState<ADDMiniGameStateBase>(); }
	/** RuleSet이 있는 경우 Initialize하는 헬퍼 */
	void InitializeRuleSet();
	/** 타이머 주기마다 미니게임 시간을 갱신하는 헬퍼 */
	void UpdateMiniGameTime();
	
protected:
	/** 런타임 미니게임 데이터 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "MiniGame")
	FMiniGameSetup ActiveSetup;
	
	/** 미니게임 참가자 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "MiniGame")
	TArray<FMiniGameParticipantInfo> ActiveParticipants;
	
	/** RuleSet이 있다면 사용 */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "MiniGame")
	TObjectPtr<UDDMiniGameRuleSet> RuleSet = nullptr;
	
	/** 미니게임 시작 여부 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "MiniGame")
	bool bIsMiniGameStarted = false;
	
	/** 미니게임 종료 여부 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "MiniGame")
	bool bIsMiniGameFinished = false;
	
	/** 경과 시간 체크 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "MiniGame")
	float ElapsedTimeSeconds = 0.0f;

	/** 시간 갱신용 타이머 핸들 */
	FTimerHandle MiniGameTimerHandle;

	/** 타이머 시간 갱신 간격 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MiniGame")
	float TimeUpdateIntervalSeconds = 0.1f;
};
