#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameModeBase.h"
#include "DDGameModeBase.generated.h"

class APlayerController;
class UAbilitySystemComponent;
class UGameplayEffect; 

UCLASS()
class ADDGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ADDGameModeBase();
    
    virtual void BeginPlay() override;
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;
    virtual void HandleSeamlessTravelPlayer(AController*& ParticipantController) override;
    virtual void GenericPlayerInitialization(AController* C) override;

    /** 상태 전환 함수 */
    void SetMatchState(FGameplayTag NewStateTag);
    
    // ==========================================
    // 턴 내부 페이즈 진행을 위한 함수들
    // ==========================================
    
    /** 캐릭터가 주사위를 던지는 애니메이션이 시작될 때 호출하여 타이머를 멈추고 스킬 발동을 차단 */
    UFUNCTION(BlueprintCallable, Category = "GameLoop|TurnPhase")
    void NotifyDiceRolled();

    /** 캐릭터 보드판 이동 연출이 끝나면 호출 */
    UFUNCTION(BlueprintCallable, Category = "GameLoop|TurnPhase")
    void NotifyMovementFinished();

protected:
    /** 1초마다 반복 실행되며 게임의 턴 시간과 대기 시간을 체크 */
    UFUNCTION()
    void OnMainTimerElapsed();
    
    /** 보드판 게임 루프 흐름 제어 함수들 */
    void CheckWinCondition();
    void StartNextPlayerTurn();
    void CalculateFinalWinner();
    
    /** 라운드 시작 시 턴 순서대로 배열을 정렬하는 함수 */
    void SortPlayersByTurnOrder();
    
    /** 플레이어의 AbilitySystemComponent를 가져오는 헬퍼 함수 */
    UAbilitySystemComponent* GetAbilitySystemComponentFromPlayer(APlayerController* PlayerController);
    
    /** 플레이어의 현재 페이즈 태그를 교체해주는 헬퍼 함수 */
    void SetTurnPhase(FGameplayTag NewPhaseTag);
    
    /** 모든 플레이어의 카메라를 특정 타겟으로 부드럽게 이동시킵니다. */
    void FocusAllCamerasOnTarget(AActor* TargetActor);
    
    /** 타이머 종료 후 실제로 다음 사람에게 턴을 넘기는 함수 */
    void ExecuteNextTurnTransition();
    
public: 
    
    virtual void OnCharacterKilled(AActor* Killer, AActor* Victim);
    
    void HandleRespawn(AController* TargetController);
    


protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData", meta=(DisplayName="본 게임에 참여 중인 플레이어 컨트롤러 목록"))
    TArray<TObjectPtr<APlayerController>> AlivePlayerControllers;

    /** 메인 게임 루프 타이머 관리 핸들 */
    FTimerHandle MainTimerHandle;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameData", meta=(DisplayName="각 턴당 주어지는 최대 제한 시간"))
    int32 MaxStateTimer = 30; 
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData", meta=(DisplayName="현재 상태에서 남은 제한 시간 (-1일 경우 타이머 일시 정지)"))
    int32 StateTimer = 0; 
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData", meta=(DisplayName="현재 누구의 턴인지 추적하는 인덱스"))
    int32 CurrentTurnPlayerIndex = 0;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameData", meta=(DisplayName="우승에 필요한 목표 트로피 개수"))
    int32 MaxTrophy = 1;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameData", meta=(DisplayName="게임이 종료되는 최대 라운드 수"))
    int32 MaxRound = 10;
    
    UPROPERTY(EditDefaultsOnly, Category = "GameData", meta=(DisplayName="N초 대기 후 다음 턴 넘기기"))
    int32 TurnTransitionTimer = 3.0f;
    
    /** N초 대기 후 다음 턴으로 넘기기 위한 타이머 핸들 */
    FTimerHandle TurnTransitionTimerHandle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Response")
    TArray<TSubclassOf<UGameplayEffect>> ReSpawnEffectClasses;

private:
    /** 상태가 전환될 때 기존 규칙을 회수하기 위해 현재 적용 중인 태그들을 기억해두는 변수 */
    FGameplayTagContainer CurrentAppliedTags;
    
    /** 현재 턴을 진행 중인 플레이어에게 부여된 세부 페이즈 태그 (페이즈 종료 시 회수 목적) */
    FGameplayTag CurrentTurnPhaseTag;
    
protected:
    /** 매치 상태(Init, PlayerTurn 등) 전환 시 적용할 GE 목록 */
    UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects|MatchState")
    TMap<FGameplayTag, TSubclassOf<UGameplayEffect>> MatchStateEffectClasses;

    /** 페이즈(BeforeDice, Moving, Event) 전환 시 적용할 GE 목록 */
    UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects|Phase")
    TMap<FGameplayTag, TSubclassOf<UGameplayEffect>> TurnPhaseEffectClasses;

    /** 현재 턴인 플레이어에게 부여할 GE */
    UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects|Turn")
    TSubclassOf<UGameplayEffect> TurnActiveEffectClass;

    /** 턴 대기 중인 플레이어에게 부여할 GE */
    UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects|Turn")
    TSubclassOf<UGameplayEffect> TurnWaitingEffectClass;

};
