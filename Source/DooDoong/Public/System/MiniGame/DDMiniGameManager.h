// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DDMiniGameManager.generated.h"

class UDDMiniGameDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMiniGameStateChanged, FGameplayTag, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMiniGameSelected, UDDMiniGameDefinition*, MiniGameDefinition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMiniGamePreparing, const FMiniGameSetup&, Setup);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMiniGameStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMiniGameFinished, const FMiniGameResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMiniGameResultCommitted, const FMiniGameResult&, Result);
/**
 * 
 */
UCLASS()
class DOODOONG_API UDDMiniGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	/** 서브시스템 생명주기 관련 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	/** 미니게임 시작 호출 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	bool RequestStartMiniGame(FName MiniGameId, const TArray<APlayerState*>& Players);

	/** 미니게임을 랜덤하게 시작하도록 호출 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	bool RequestStartRandomMiniGame(const TArray<APlayerState*>& Players);

	/** 미니게임 종료 호출 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	bool RequestFinishMiniGame();

public:
	/** 활성화된 미니게임을 Reason Tag에 의해 종료 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	void StopActiveMiniGame(FGameplayTag Reason);
	
	/** 미니게임 결과를 커밋 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	void CommitMiniGameResult(const FMiniGameResult& Result);

	/** 미니게임이 시작되었음을 알리고 상태 변경 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	void NotifyMiniGameStarted();

public:
	/** 현재 실행 상태 조회 */
	UFUNCTION(BlueprintPure, Category="MiniGame")
	FGameplayTag GetCurrentState() const { return CurrentState; }

	/** 현재 활성화 된 게임의 정보를 조회 */
	UFUNCTION(BlueprintPure, Category="MiniGame")
	const FMiniGameSetup& GetActiveSetup() const { return ActiveSetup; }

	/** 현재 활성화 된 게임에 참가 중인 참가자 조회 */
	UFUNCTION(BlueprintPure, Category="MiniGame")
	const TArray<FMiniGameParticipantInfo>& GetActiveParticipants() const { return ActiveParticipants; }

	/** 현재 활성화 된 게임의 데이터 에셋 Get */
	UFUNCTION(BlueprintPure, Category="MiniGame")
	UDDMiniGameDefinition* GetActiveDefinition() const { return ActiveDefinition; }

public:
	/** 활성화된 미니게임이 있는지 확인 */
	UFUNCTION(BlueprintPure, Category="MiniGame")
	bool HasActiveMiniGame() const { return ActiveDefinition != nullptr; }

	/** 마지막으로 진행한 미니게임에서 커밋된 결과를 조회 */
	UFUNCTION(BlueprintPure, Category="MiniGame")
	const FMiniGameResult& GetLastCommittedResult() const { return LastCommittedResult; }
	
	/** 마지막으로 미니게임을 한 적이 있는지, 즉 결과가 존재하는지 확인 */
	UFUNCTION(BlueprintPure, Category="MiniGame")
	bool HasLastCommittedResult() const { return bHasLastCommittedResult; }

	/** 저장된 마지막 미니게임 결과를 클리어 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	void ClearLastCommittedResult();

	/** 미니게임 정의 조회 */
	UFUNCTION(BlueprintCallable, Category="MiniGame")
	UDDMiniGameDefinition* FindMiniGameDefinition(FName MiniGameId);

	/** 현재 월드가 활성화 된 미니게임 월드인지 판별 */
	bool IsActiveMiniGameWorld(const UWorld* World) const;

public:
	/** 상태 변화 델리게이트 메서드 모음 */
	
	UPROPERTY(BlueprintAssignable, Category="MiniGame")
	FOnMiniGameSelected OnMiniGameSelected;
	UPROPERTY(BlueprintAssignable, Category="MiniGame")
	FOnMiniGamePreparing OnMiniGamePreparing;
	UPROPERTY(BlueprintAssignable, Category="MiniGame")
	FOnMiniGameStarted OnMiniGameStarted;
	UPROPERTY(BlueprintAssignable, Category="MiniGame")
	FOnMiniGameStateChanged OnMiniGameStateChanged;
	UPROPERTY(BlueprintAssignable, Category="MiniGame")
	FOnMiniGameFinished OnMiniGameFinished;
	UPROPERTY(BlueprintAssignable, Category="MiniGame")
	FOnMiniGameResultCommitted OnMiniGameResultCommitted;

protected:
	/** 내부 상태 전환 및 세팅 구성 */
	void SetCurrentState(FGameplayTag NewState);
	bool BuildSetupFromDefinition(const UDDMiniGameDefinition* Definition, const TArray<APlayerState*>& Players);

	/** 런타임 데이터 초기화와 맵 이동 처리 */
	void ClearActiveSession();
	bool TravelToMap(const TSoftObjectPtr<UWorld>& MapAsset);

	/** 데이터 에셋 검색 및 캐시 */
	void CacheAvailableDefinitions();
	
protected:
	/** 현재 게임의 상태 */
	UPROPERTY(VisibleInstanceOnly, Category="MiniGame", meta=(Categories="MiniGame.State"))
	FGameplayTag CurrentState = DDMiniGameplayTags::State_Idle;

	/** 현재 실행중인 게임의 정보 */
	UPROPERTY(VisibleInstanceOnly, Category="MiniGame")
	FMiniGameSetup ActiveSetup;

	/** 현재 게임에 참여 중인 참가자 */
	UPROPERTY(VisibleInstanceOnly, Category="MiniGame")
	TArray<FMiniGameParticipantInfo> ActiveParticipants;

	/** 미니 게임의 데이터 에셋 */
	UPROPERTY(VisibleInstanceOnly, Category="MiniGame")
	TObjectPtr<UDDMiniGameDefinition> ActiveDefinition = nullptr;

	/** 마지막 게임의 결과 커밋을 담는 구조체 */
	UPROPERTY(VisibleInstanceOnly, Category="MiniGame")
	FMiniGameResult LastCommittedResult;

	/** 마지막 결과가 있는지 여부 */
	UPROPERTY(VisibleInstanceOnly, Category="MiniGame")
	bool bHasLastCommittedResult = false;

	/** 존재하는 모든 미니게임 데이터 캐시 */
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UDDMiniGameDefinition>> CachedDefinitions;

	/**	되돌아갈 맵의 이름 저장 */
	UPROPERTY()
	FString ReturnMapPackageName;
};
