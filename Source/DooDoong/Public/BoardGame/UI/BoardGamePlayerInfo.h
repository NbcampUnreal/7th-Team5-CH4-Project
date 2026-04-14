#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoardGamePlayerInfo.generated.h"

class UTextBlock; 

UCLASS()
class DOODOONG_API UBoardGamePlayerInfo : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerNameText; 
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TrophyText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CoinText;
	
	
};
