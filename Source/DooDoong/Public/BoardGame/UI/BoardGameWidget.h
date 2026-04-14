#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoardGameWidget.generated.h"

class UBoardGamePlayerInfo;
class UTextBlock;

UCLASS()
class DOODOONG_API UBoardGameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void UpdateTimeText(int32 InTime);
	
	UFUNCTION()
	void UpdateRemainTurn(int32 InRemainTurn); 
	
	
protected:
	UPROPERTY(meta =(BindWidget))
	UTextBlock* RemainTimeText;
	
	UPROPERTY(meta =(BindWidget))
	UTextBlock* RemainTurnText;
	
	UPROPERTY(meta =(BindWidget))
	UTextBlock* MyTurnText;
	
	UPROPERTY(meta =(BindWidget))
	TArray<UBoardGamePlayerInfo*> PlayerInfos;
	
};
