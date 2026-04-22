#pragma once

#include "CoreMinimal.h"
#include "DDMiniGameTypes.h"
#include "UObject/Object.h"
#include "DDMiniGameRuleSet.generated.h"

class ADDMiniGameStateBase;
UCLASS()
class DOODOONG_API UDDMiniGameRuleSet : public UObject
{
	GENERATED_BODY()
	
public:
	/** 룰셋 초기화 */
	UFUNCTION(BlueprintCallable, Category="MiniGame Rules")
	virtual void InitializeRules(const FMiniGameSetup& InSetup);

	/** 종료 조건 판단 */
	UFUNCTION(BlueprintCallable, Category="MiniGame Rules")
	virtual bool ShouldFinishGame(const ADDMiniGameStateBase* GameState) const;

	/** 점수 처리 */
	UFUNCTION(BlueprintCallable, Category="MiniGame Rules")
	virtual void AddScore(ADDMiniGameStateBase* GameState, APlayerState* PlayerState, int32 DeltaScore);

	/** 점수에 따라 순위를 확정하는 부분 */
	UFUNCTION(BlueprintCallable, Category="MiniGame Rules")
	virtual void ResolveRanking(ADDMiniGameStateBase* GameState) const;

	/** 게임 결과를 구조체로 만드는 부분 */
	UFUNCTION(BlueprintCallable, Category="MiniGame Rules")
	virtual FMiniGameResult BuildMiniGameResult(const ADDMiniGameStateBase* GameState) const;

protected:
	/** 미니 게임 데이터 캐시 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MiniGame Rules")
	FMiniGameSetup CachedSetup;
};
