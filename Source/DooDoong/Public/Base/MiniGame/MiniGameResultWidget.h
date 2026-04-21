#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniGameResultWidget.generated.h"

class ADDMiniGameStateBase;
class UTextBlock;
class UHorizontalBox;
class UDDMiniGameRankEntryWidget;

UCLASS()
class DOODOONG_API UMiniGameResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION(BlueprintCallable, Category="MiniGame|Result")
	void InitializeResult(const TArray<FMiniGameScoreEntry>& ScoreBoard);
	
protected:
	UFUNCTION()
	void OnMiniGameStateChanged(FGameplayTag StateTag);

private:
	void PopulateRankEntries(const TArray<FMiniGameScoreEntry>& Sorted);

protected:
	/** 1등 이름 ("1등 : 플레이어 이름") */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> WinnerNameText;

	/** 2~4등 동적 항목이 들어가는 컨테이너 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> PlayerNameContainer;

	/** 각 랭크 항목 위젯 클래스 — 블루프린트에서 WBP_MiniGameRankEntry 지정 */
	UPROPERTY(EditDefaultsOnly, Category="MiniGame|Result")
	TSubclassOf<UDDMiniGameRankEntryWidget> RankEntryWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<ADDMiniGameStateBase> CachedMiniGameState;
};
