#pragma once

#include "CoreMinimal.h"
#include "DDMiniGameTypes.h"
#include "Base/Game/DDGameStateBase.h"
#include "DDMiniGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMiniGameReadyStateChanged, int32, ReadyPlayerCount, int32, TotalParticipantCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMiniGameReadyEntriesChanged, const TArray<FMiniGameReadyEntry>&, ReadyEntries);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMiniGameScoreBoardChanged);

/**
 * 
 */
UCLASS()
class DOODOONG_API ADDMiniGameStateBase : public ADDGameStateBase
{
	GENERATED_BODY()
	
public:
	ADDMiniGameStateBase();

	/** 복제 설정 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** 진행 상태 */
	UFUNCTION(BlueprintPure, Category="MiniGame")
	FGameplayTag GetMiniGameState() const { return CurrentState; }
	
	/** 미니게임 상태를 갱신 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	void SetMiniGameState(FGameplayTag NewState);
	
	/** 미니 게임 상태가 해당하는 상태인지 확인하는 헬퍼 */
	UFUNCTION(BlueprintPure, Category="MiniGame")
	bool IsMiniGameState(FGameplayTag StateTag) const { return CurrentState == StateTag; }

public:
	/** 남은 시간 Get */
	UFUNCTION(BlueprintPure, Category="MiniGame")
	float GetRemainingTimeSeconds() const { return RemainingTimeSeconds; }
	
	/** 남은 시간 Set */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	void SetRemainingTimeSeconds(float NewRemainingTimeSeconds);

	/** 참가자 정보 Get */
	UFUNCTION(BlueprintPure, Category="MiniGame")
	const TArray<FMiniGameParticipantInfo>& GetParticipants() const { return Participants; }
	/** 참가자 정보 Set */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	void SetParticipants(const TArray<FMiniGameParticipantInfo>& InParticipants);

	/** 점수판 정보 Get */
	UFUNCTION(BlueprintPure, Category="MiniGame")
	const TArray<FMiniGameScoreEntry>& GetScoreBoard() const { return ScoreBoard; }
	/** 점수판 정보 Set */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	void SetScoreBoard(const TArray<FMiniGameScoreEntry>& InScoreBoard);

	/** 준비 완료 인원 수 Get */
	UFUNCTION(BlueprintPure, Category="MiniGame|Ready")
	int32 GetReadyPlayerCount() const { return ReadyPlayerCount; }

	/** 전체 참가자 수 Get */
	UFUNCTION(BlueprintPure, Category="MiniGame|Ready")
	int32 GetTotalParticipantCount() const { return TotalParticipantCount; }

	/** 준비 완료 인원 수 Set */
	UFUNCTION(BlueprintCallable, Category="MiniGame|Ready")
	void SetReadyPlayerCount(int32 NewReadyPlayerCount);

	/** 전체 참가자 수 Set */
	UFUNCTION(BlueprintCallable, Category="MiniGame|Ready")
	void SetTotalParticipantCount(int32 NewTotalParticipantCount);

	/** 플레이어별 준비 상태 목록 Get */
	UFUNCTION(BlueprintPure, Category="MiniGame|Ready")
	const TArray<FMiniGameReadyEntry>& GetReadyEntries() const { return ReadyEntries; }

	/** 플레이어별 준비 상태 목록 Set */
	UFUNCTION(BlueprintCallable, Category="MiniGame|Ready")
	void SetReadyEntries(const TArray<FMiniGameReadyEntry>& InReadyEntries);
	
public:
	/** 점수 추가 로직 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	void AddScore(APlayerState* PlayerState, int32 DeltaScore);
	
	/** 점수 Get */
	UFUNCTION(BlueprintPure, Category="MiniGame")
	int32 GetScore(APlayerState* PlayerState) const;

public:
	/** 준비 상태 UI 갱신을 위한 델리게이트 */
	UPROPERTY(BlueprintAssignable, Category="MiniGame|Ready")
	FOnMiniGameReadyStateChanged OnMiniGameReadyStateChanged;

	/** 플레이어별 준비 상태 UI 갱신을 위한 델리게이트 */
	UPROPERTY(BlueprintAssignable, Category="MiniGame|Ready")
	FOnMiniGameReadyEntriesChanged OnMiniGameReadyEntriesChanged;

	/** 플레이어의 ScoreBoard 갱신 시 UI도 갱신하기 위한 델리게이트 */
	UPROPERTY(BlueprintAssignable, Category="MiniGame|ScoreBoard")
	FOnMiniGameScoreBoardChanged OnMiniGameScoreBoardChanged;

public:
	/** 준비 인원 수가 클라이언트에 동기화되면 UI를 갱신하기 위해 호출 */
	UFUNCTION()
	void OnRep_ReadyPlayerCount();

	/** 서버에서 전체 참가자 수가 클라이언트에 동기화되면 UI를 갱신하기 위해 호출 */
	UFUNCTION()
	void OnRep_TotalParticipantCount();

	/** 준비 상태가 바뀌어서 클라이언트에 동기화되면 UI 갱신하기 위해 호출 */
	UFUNCTION()
	void OnRep_ReadyEntries();
	
	/** ScoreBoard가 갱신되면 UI도 갱신하기 위함 */
	UFUNCTION()
	void OnRep_ScoreBoard();

public:
	/** 준비상태 변화 헬퍼 */
	void BroadcastReadyStateChanged();
	
	/** 준비상태 변화 헬퍼2 */
	void BroadcastReadyEntriesChanged();
	
	/** ScoreBoard 변화 헬퍼 */
	void BroadcastScoreBoardChanged();

protected:
	/** 현재 게임 상태 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="MiniGame", meta=(Categories="MiniGame.State"))
	FGameplayTag CurrentState = DDGameplayTags::State_MiniGame_Idle;
	
	/** 남은 시간 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="MiniGame")
	float RemainingTimeSeconds = 0.f;
	
	/** 참가자 정보를 담은 배열 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="MiniGame")
	TArray<FMiniGameParticipantInfo> Participants;
	
	/** 점수판 */
	UPROPERTY(ReplicatedUsing=OnRep_ScoreBoard, VisibleAnywhere, BlueprintReadOnly, Category="MiniGame")
	TArray<FMiniGameScoreEntry> ScoreBoard;

	/** 준비 완료한 참가자 수 */
	UPROPERTY(ReplicatedUsing=OnRep_ReadyPlayerCount, VisibleAnywhere, BlueprintReadOnly, Category="MiniGame|Ready")
	int32 ReadyPlayerCount = 0;

	/** 준비 대상인 전체 참가자 수 */
	UPROPERTY(ReplicatedUsing=OnRep_TotalParticipantCount, VisibleAnywhere, BlueprintReadOnly, Category="MiniGame|Ready")
	int32 TotalParticipantCount = 0;

	/** 플레이어별 준비 상태 */
	UPROPERTY(ReplicatedUsing=OnRep_ReadyEntries, VisibleAnywhere, BlueprintReadOnly, Category="MiniGame|Ready")
	TArray<FMiniGameReadyEntry> ReadyEntries;
};
