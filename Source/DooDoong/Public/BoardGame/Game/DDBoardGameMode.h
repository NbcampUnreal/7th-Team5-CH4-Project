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
	virtual void HandleRespawn(AController* TargetController) override;

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
	
	// ==========================================
    // 타이머 틱 분리 핸들러
    // ==========================================
    
    /** 모든 플레이어가 접속하고 PlayerState가 유효한지 매 틱마다 확인하여, 충족되면 Init 상태로 전환하는 함수 */
    void TickState_WaitingForPlayers();

    /** 현재 턴인 플레이어의 제한 시간을 매 틱마다 깎고, 시간이 0이 되면 다음 턴으로 강제 전환시키는 함수 */
    void TickState_PlayerTurn();

    /** 라운드 종료 시 대기 시간을 깎고, 시간이 0이 되면 남은 플레이어들을 미니게임 맵으로 이동시키는 함수 */
    void TickState_RoundEnd();

    
    // ==========================================
    // 상태 전환 로직 분리
    // ==========================================
    
    /** 게임 최초 진입 시 타일 초기화, 플레이어 위치 배정, 1라운드 인트로 시퀀스 재생 등을 총괄하는 함수 */
    void HandleState_Init();

    /** 라운드 종료 시 누군가 승리 조건을 달성했는지 선제 검사하고, 승자가 없으면 미니게임 진입 전 대기 및 타일 저장을 수행하는 함수 */
    void HandleState_RoundEnd();

    /** 최종 순위까지 모두 계산된 후, 일정 시간(예: 10초) 대기 후 로비로 돌아가는 타이머를 작동시키는 함수 */
    void HandleState_End();


    // ==========================================
    // 핵심 게임 로직
    // ==========================================
    
    /** 목표 트로피 개수 도달자가 있는지, 또는 최대 라운드에 도달했는지 검사하여 Ending 페이즈 진입 여부를 판정하는 함수 */
    void CheckWinCondition();

    /** 다음 턴 인덱스로 넘어가며 타이머를 초기화하고, 해당 플레이어에게만 마우스/UI 제어권을 부여하는 턴 시작 함수 */
    void StartNextPlayerTurn();

    /** 모든 플레이어의 턴이 1회씩 종료되었을 때, 턴 인덱스를 초기화하고 현재 라운드를 증가시킨 뒤 RoundEnd 상태로 전환하는 함수 */
    void ProcessRoundTransition();

    /** 게임 종료 시 트로피(1순위)와 코인(2순위)을 비교하여 플레이어들의 최종 순위를 정렬하고, 그 결과를 GameState에 저장하는 함수 */
    void CalculateFinalWinner();

    /** 미니게임 등을 통해 획득한 턴 우선순위(TurnOrder)를 기준으로 플레이어 컨트롤러 배열을 정렬하여 순번을 확정하는 함수 */
    void SortPlayersByTurnOrder();

    /** 주사위 대기, 이동 중 등 턴 내부의 세부 페이즈 태그를 갱신하고, 해당 페이즈에 맞는 GAS 이펙트를 부여하는 함수 */
    void SetTurnPhase(FGameplayTag NewPhaseTag);

    /** 이동 연출이나 아이템 사용이 완전히 끝난 후, 약간의 대기 시간을 거쳐 실제로 다음 플레이어에게 턴을 넘기는 딜레이용 함수 */
    void ExecuteNextTurnTransition();

    
    // ==========================================
    // 유틸 및 보조 
    // ==========================================
    
    /** 새로운 MatchState(Init, PlayerTurn, RoundEnd 등)로 전환될 때, 그 상태에 대응하는 공통 GAS 이펙트를 모든 플레이어에게 적용하는 함수 */
    void ApplyMatchStateEffects(FGameplayTag NewStateTag);

    /** 현재 턴의 주인에게는 '턴 활성화' 이펙트를, 나머지 대기자들에게는 '턴 대기' 이펙트를 갱신하여 부여하는 함수 */
    void ApplyTurnEffectsToPlayers();

    /** ServerTravel을 호출하여 서버와 모든 클라이언트를 묶어 로비 맵으로 이동시키는 함수 */
    void TravelToLobby();
	
	// ==========================================
    // UI
    // ==========================================
	void UpdateAllPlayersGameLayer(FGameplayTag StateTag);

protected:
	// 캐싱 최적화
	UPROPERTY()
    class ADDBoardGameState* CachedBoardGameState;

	/** 메인 게임 루프 타이머 관리 핸들 */
	FTimerHandle MainTimerHandle;
	FTimerHandle TurnTransitionTimerHandle;
	FTimerHandle ReturnToLobbyTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "GameData", meta=(DisplayName="게임 엔딩 시 돌아갈 로비 맵"))
    FString LobbyMapPath = TEXT("/Game/DooDoong/Map/L_Park?listen");
	
	UPROPERTY(EditDefaultsOnly, Category = "GameData", meta=(DisplayName="N초 대기 후 다음 턴 넘기기"))
	float TurnTransitionTimer = 3.0f;

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
