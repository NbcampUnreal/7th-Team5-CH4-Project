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

    /** 상점 이용 등 발판 이벤트 처리가 모두 끝나면 호출 */
    UFUNCTION(BlueprintCallable, Category = "GameLoop|TurnPhase")
    void NotifyTileEventFinished();

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
    
public: 
    
    virtual void OnCharacterKilled(AActor* Killer, AActor* Victim);
    
    void HandleRespawn(AController* TargetController);

protected:
    /** 본 게임에 참여 중인 플레이어 컨트롤러 목록 (관전자 제외, 턴 관리에 사용) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData")
    TArray<TObjectPtr<APlayerController>> AlivePlayerControllers;

    /** 메인 게임 루프 타이머 관리 핸들 */
    FTimerHandle MainTimerHandle;

    /** 각 턴당 주어지는 최대 제한 시간 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameData")
    int32 MaxStateTimer = 30; 
    
    /** 현재 상태에서 남은 제한 시간 (-1일 경우 타이머 일시 정지) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData")
    int32 StateTimer = 0; 

    /** 현재 누구의 턴인지 추적하는 인덱스 (AlivePlayerControllers 기준) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData")
    int32 CurrentTurnPlayerIndex = 0;

    /** 상태 태그가 바뀔 때 자동으로 적용할 부가 규칙 태그 매핑 데이터 */
    UPROPERTY(EditDefaultsOnly, Category = "GAS|MatchState")
    TMap<FGameplayTag, FGameplayTagContainer> StateTagMapping;
    
    /** 게임 종료 조건 1: 우승에 필요한 목표 트로피 개수 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameData")
    int32 MaxTrophy = 1;
    
    /** 게임 종료 조건 2: 게임이 종료되는 최대 라운드 수 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameData")
    int32 MaxRound = 10;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Response")
    TArray<TSubclassOf<UGameplayEffect>> ReSpawnEffectClasses;
    
private:
    /** 상태가 전환될 때 기존 규칙을 회수하기 위해 현재 적용 중인 태그들을 기억해두는 변수 */
    FGameplayTagContainer CurrentAppliedTags;
    
    /** 현재 턴을 진행 중인 플레이어에게 부여된 세부 페이즈 태그 (페이즈 종료 시 회수 목적) */
    FGameplayTag CurrentTurnPhaseTag;
};
