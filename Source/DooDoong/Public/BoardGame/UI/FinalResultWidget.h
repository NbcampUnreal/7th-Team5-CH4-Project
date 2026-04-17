#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FinalResultWidget.generated.h"

class UTextBlock;

UCLASS()
class DOODOONG_API UFinalResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerName1;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TrophyCount1;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CoinCount1;
	
protected:	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerName2;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TrophyCount2;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CoinCount2;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerName3;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TrophyCount3;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CoinCount3;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerName4;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TrophyCount4;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CoinCount4;
};
