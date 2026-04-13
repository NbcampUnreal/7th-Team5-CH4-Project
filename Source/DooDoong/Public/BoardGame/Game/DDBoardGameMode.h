#pragma once

#include "CoreMinimal.h"
#include "Base/Game/DDGameModeBase.h"
#include "DDBoardGameMode.generated.h"

UCLASS()
class DOODOONG_API ADDBoardGameMode : public ADDGameModeBase
{
	GENERATED_BODY()

public:
	ADDBoardGameMode();

	virtual void BeginPlay() override;
	virtual void SetMatchState(FGameplayTag NewStateTag) override;
	virtual void Logout(AController* Exiting) override;

	// ==========================================
	// 턴 내부 페이즈 진행을 위한 함수들
	// ==========================================
	UFUNCTION(BlueprintCallable, Category = "GameLoop|TurnPhase")
	void NotifyDiceRolled();

	UFUNCTION(BlueprintCallable, Category = "GameLoop|TurnPhase")
	void NotifyMovementFinished();

protected:
	UFUNCTION()
	void OnMainTimerElapsed();

	void CheckWinCondition();
	void StartNextPlayerTurn();
	void CalculateFinalWinner();
	void SortPlayersByTurnOrder();
	void SetTurnPhase(FGameplayTag NewPhaseTag);
	void ExecuteNextTurnTransition();

protected:
	/** 메인 게임 루프 타이머 관리 핸들 */
	FTimerHandle MainTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameData", meta=(DisplayName="각 턴당 주어지는 최대 제한 시간"))
	int32 MaxStateTimer = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData", meta=(DisplayName="현재 상태에서 남은 제한 시간"))
	int32 StateTimer = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData", meta=(DisplayName="현재 누구의 턴인지 추적하는 인덱스"))
	int32 CurrentTurnPlayerIndex = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameData", meta=(DisplayName="우승에 필요한 목표 트로피 개수"))
	int32 MaxTrophy = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameData", meta=(DisplayName="게임이 종료되는 최대 라운드 수"))
	int32 MaxRound = 10;

	UPROPERTY(EditDefaultsOnly, Category = "GameData", meta=(DisplayName="N초 대기 후 다음 턴 넘기기"))
	int32 TurnTransitionTimer = 3.0f;

	FTimerHandle TurnTransitionTimerHandle;

private:
	FGameplayTagContainer CurrentAppliedTags;
	FGameplayTag CurrentTurnPhaseTag;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects|MatchState")
	TMap<FGameplayTag, TSubclassOf<UGameplayEffect>> MatchStateEffectClasses;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects|Phase")
	TMap<FGameplayTag, TSubclassOf<UGameplayEffect>> TurnPhaseEffectClasses;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects|Turn")
	TSubclassOf<UGameplayEffect> TurnActiveEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects|Turn")
	TSubclassOf<UGameplayEffect> TurnWaitingEffectClass;
};
