#pragma once

#include "CoreMinimal.h"
#include "DDMiniGameStateBase.h"
#include "DDMiniGameTypes.h"
#include "GameFramework/GameModeBase.h"
#include "DDMiniGameModeBase.generated.h"

class ADDBasePlayerController;
class ADDMiniGameSpawnPoint;
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
	
	/** Seamless Travel 직후 플레이어별 Pawn과 시작 위치를 공통으로 처리하기 위한 전용 함수 */
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	
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
	/** Seamless Travel 시점에도 BeginPlay 전에 참가자 정보가 필요할 수 있어 미니게임 데이터를 로드 */
	void LoadActiveMiniGameData();
	
	/** 현재 PlayerState에 대응되는 참가자 정보를 찾음 */
	const FMiniGameParticipantInfo* FindParticipantInfo(const APlayerState* PlayerState) const;
	
	/** 슬롯 번호에 대응되는 스폰 포인트를 찾음 */
	ADDMiniGameSpawnPoint* FindSpawnPointBySlotIndex(int32 SlotIndex) const;
	
	/** 관전자 처리를 미니게임별로 쉽게 교체할 수 있도록 분리 */
	virtual bool ShouldSpawnAsSpectator(const FMiniGameParticipantInfo& Participant) const;
	
	/** 미니게임별 Pawn 타입 교체를 쉽게 하기 위한 함수 */
	virtual TSubclassOf<APawn> ResolvePawnClass(const FMiniGameParticipantInfo& Participant) const;
	
	/** 기본은 SpawnPoint 액터의 위치에서 스폰되도록, 특수한 경우라면 override 하면 되도록 분리 */
	virtual FTransform ResolveSpawnTransform(const FMiniGameParticipantInfo& Participant);
	
	/** 참가자용 Pawn 스폰 및 Possess 공통 처리 */
	UFUNCTION(BlueprintCallable, Category="MiniGame|Spawn")
	virtual void SpawnParticipantPawn(APlayerController* PlayerController, const FMiniGameParticipantInfo& Participant);
	
	/** 비참가자 혹은 연결이 끊기거나 여타 예외상황이 발생한 플레이어용 Spectator 스폰*/
	UFUNCTION(BlueprintCallable, Category="MiniGame|Spawn")
	virtual void SpawnSpectatorPawn(APlayerController* PlayerController);
	
protected:
	/** MiniGameStateBase를 가져오는 헬퍼 */
	TObjectPtr<ADDMiniGameStateBase> GetMiniGameState() const { return GetGameState<ADDMiniGameStateBase>(); }
	
	/** 미니게임에서 사용할 Input을 적용하는 헬퍼 */
	void ApplyMiniGameInput(ADDBasePlayerController* PlayerController);
	
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
