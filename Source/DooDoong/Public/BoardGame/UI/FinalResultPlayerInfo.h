#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FinalResultPlayerInfo.generated.h"

class UTextBlock; 
struct FFinalRankData; 

UCLASS()
class DOODOONG_API UFinalResultPlayerInfo : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializePlayerInfo(const FFinalRankData& RankData);
	
protected:
	UPROPERTY(meta =(BindWidget))
	UTextBlock* RankText; 

	UPROPERTY(meta =(BindWidget))
	UTextBlock* PlayerNameText; 
	
	UPROPERTY(meta =(BindWidget))
	UTextBlock* TrophyText; 
	
	UPROPERTY(meta =(BindWidget))
	UTextBlock* CoinText; 
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLinearColor> RankColors = {FLinearColor::White,};
	
};
