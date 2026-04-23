#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Base/Game/DDGameStateBase.h"
#include "DDBoardGameState.generated.h"

class ADDTileManager;
class ULevelSequence;

// 결과 전달 구조체: 탈주, 동점자 UI에러 방지 목적
USTRUCT(BlueprintType)
struct FFinalRankData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RankData")
	int32 Rank = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RankData")
	FName PlayerName = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RankData")
	int32 Trophy = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RankData")
	int32 Coin = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RankData")
	FLinearColor PlayerColor = FLinearColor::White;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFinalRankingsUpdated, const TArray<FFinalRankData>&, Rankings);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateTimerChanged, int32, NewTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundChanged, int32, NewRound);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerArrayChanged);

class ALevelSequenceActor;

UCLASS()
class DOODOONG_API ADDBoardGameState : public ADDGameStateBase
{
	GENERATED_BODY()

public:
	ADDBoardGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

public:
	// ==========================================
	// FORCEINLINE Getter / Setter
	// ==========================================

	FORCEINLINE FGameplayTag GetBoardMatchState() const { return BoardMatchStateTag; }
	FORCEINLINE void SetBoardMatchState(FGameplayTag NewState) { BoardMatchStateTag = NewState; }

	FORCEINLINE int32 GetTurnPlayerIndex() const { return TurnPlayerIndex; }
	FORCEINLINE void SetTurnPlayerIndex(int32 NewIndex) { TurnPlayerIndex = NewIndex; }

	FORCEINLINE int32 GetStateTimer() const { return StateTimer; }
	FORCEINLINE void SetStateTimer(int32 NewTimer) { StateTimer = NewTimer; }
	FORCEINLINE void DecreaseStateTimer() { if (StateTimer > 0) StateTimer--; }

	FORCEINLINE int32 GetCurrentRound() const { return CurrentRound; }
	FORCEINLINE void SetCurrentRound(int32 NewRound) { CurrentRound = NewRound; }

	FORCEINLINE void SetFinalRankings(const TArray<FFinalRankData>& InRankings) { FinalRankings = InRankings; }

	FORCEINLINE int32 GetMinPlayerCount() const { return MinPlayerCount; }
	
	FORCEINLINE TArray<FFinalRankData> GetFinalRankings() const { return FinalRankings; }
public:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	
public:
	// --- 고정 데이터 (Read-Only) ---
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "GameData", meta=(DisplayName="플레이어 턴 최소 인원 수"))
	int32 MinPlayerCount = 2;
	
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "GameData", meta=(DisplayName="각 플레이어 턴 시간"))
	int32 MaxStateTimer = 60;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "GameData", meta=(DisplayName="목표 트로피 수"))
	int32 MaxTrophy = 1;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "GameData", meta=(DisplayName="종료 라운드"))
	int32 MaxRound = 10;

protected:
	// --- 동기화 데이터 (직접 접근 금지) ---
	UPROPERTY(Replicated)
	FGameplayTag BoardMatchStateTag;

	UPROPERTY(Replicated)
	int32 TurnPlayerIndex = -1;

	UPROPERTY(ReplicatedUsing = OnRep_StateTimer)
	int32 StateTimer = 0;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentRound)
	int32 CurrentRound = 0;

	UPROPERTY(ReplicatedUsing = OnRep_FinalRankings)
	TArray<FFinalRankData> FinalRankings;

public:
	// --- 델리게이트 ---
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStateTimerChanged OnStateTimerChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRoundChanged OnRoundChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnFinalRankingsUpdated OnFinalRankingsUpdated;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerArrayChanged OnPlayerArrayChanged;

protected:
	UFUNCTION()
	void OnRep_StateTimer();

	UFUNCTION()
	void OnRep_CurrentRound();

	UFUNCTION()
	void OnRep_FinalRankings();

public:
	// --- 레벨 시퀀스 --- 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
    TSoftObjectPtr<ULevelSequence> BoardIntroSequenceAsset;
	
    UPROPERTY(Transient)
    TObjectPtr<ALevelSequenceActor> SpawnedSequenceActor;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySequence();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayBoardBGM();

	UFUNCTION()
	void OnSequenceFinished();

public:
	UFUNCTION()
	void PlayBoardBGMLocal();
	
	/** 실시간 랭킹을 갱신하고, 정렬된 플레이어 배열을 반환하는 공통 함수 */
	UFUNCTION(BlueprintCallable, Category = "Ranking")
    TArray<class ADDBasePlayerState*> UpdateAndGetPlayerRanks();

	UPROPERTY()
	ADDTileManager* TileManager;
};
