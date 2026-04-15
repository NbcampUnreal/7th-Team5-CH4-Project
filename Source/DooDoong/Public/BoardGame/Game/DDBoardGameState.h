#pragma once

#include "CoreMinimal.h"
#include "Base/Game/DDGameStateBase.h"
#include "DDBoardGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateTimerChanged, int32, NewTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundChanged, int32, NewRound);

UCLASS()
class DOODOONG_API ADDBoardGameState : public ADDGameStateBase
{
	GENERATED_BODY()

public:
	ADDBoardGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// --- 동기화 데이터 (DDBoardGameMode에서 세팅) ---
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "GameData",
		meta=(DisplayName="각 턴당 주어지는 최대 제한 시간"))
	int32 MaxStateTimer = 30;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "GameData",
		meta=(DisplayName="우승에 필요한 목표 트로피 개수"))
	int32 MaxTrophy = 1;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "GameData",
		meta=(DisplayName="게임이 종료되는 최대 라운드 수"))
	int32 MaxRound = 10;

	// --- OnRep이 필요한 동적 데이터 ---
	UPROPERTY(ReplicatedUsing = OnRep_StateTimer, VisibleAnywhere, BlueprintReadOnly, Category = "GameData",
		meta=(DisplayName="현재 상태에서 남은 제한 시간"))
	int32 StateTimer = 0;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentRound, VisibleAnywhere, BlueprintReadOnly, Category = "GameData")
	int32 CurrentRound = 1;

	// --- 이벤트 델리게이트 (위젯에서 바인딩) ---
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStateTimerChanged OnStateTimerChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRoundChanged OnRoundChanged;

protected:
	UFUNCTION()
	void OnRep_StateTimer();

	UFUNCTION()
	void OnRep_CurrentRound();
};
