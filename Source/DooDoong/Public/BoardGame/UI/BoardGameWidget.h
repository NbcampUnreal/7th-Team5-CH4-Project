#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoardGameWidget.generated.h"

class ADDBoardGameState;
class UBoardGamePlayerInfo;
class UTextBlock;

UCLASS()
class DOODOONG_API UBoardGameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	void BindToGameState();

	UFUNCTION()
	void UpdateTimeText(int32 InTime);
	
	UFUNCTION()
	void UpdateRemainTurn(int32 InRemainTurn); 
	
	
protected:
	UPROPERTY(meta =(BindWidget))
	UTextBlock* RemainTimeText;
	
	UPROPERTY(meta =(BindWidget))
	UTextBlock* RemainTurnText;
	
	// UPROPERTY(meta =(BindWidget))
	// TArray<UBoardGamePlayerInfo*> PlayerInfos;
	
	UPROPERTY()
	TObjectPtr<ADDBoardGameState> CurrentGameState;
	
};
