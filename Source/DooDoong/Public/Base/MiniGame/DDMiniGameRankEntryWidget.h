#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDMiniGameRankEntryWidget.generated.h"

class UTextBlock;
struct FMiniGameScoreEntry;

UCLASS()
class DOODOONG_API UDDMiniGameRankEntryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable, Category="MiniGame|Result")
	void SetEntryData(const FMiniGameScoreEntry& Entry);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> RankText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> NameText;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> ScoreText;	
};
