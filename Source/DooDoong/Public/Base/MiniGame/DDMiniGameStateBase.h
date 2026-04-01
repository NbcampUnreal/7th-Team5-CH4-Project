#pragma once

#include "CoreMinimal.h"
#include "DDMiniGameTypes.h"
#include "GameFramework/GameStateBase.h"
#include "DDMiniGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class DOODOONG_API ADDMiniGameStateBase : public AGameStateBase
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
	
public:
	/** 점수 추가 로직 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	void AddScore(APlayerState* PlayerState, int32 DeltaScore);
	/** 점수 Get */
	UFUNCTION(BlueprintPure, Category="MiniGame")
	int32 GetScore(APlayerState* PlayerState) const;

protected:
	/** 현재 게임 상태 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="MiniGame", meta=(Categories="MiniGame.State"))
	FGameplayTag CurrentState = DDMiniGameplayTags::State_Idle;
	
	/** 남은 시간 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="MiniGame")
	float RemainingTimeSeconds = 0.f;
	
	/** 참가자 정보를 담은 배열 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="MiniGame")
	TArray<FMiniGameParticipantInfo> Participants;
	
	/** 점수판 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="MiniGame")
	TArray<FMiniGameScoreEntry> ScoreBoard;
};
