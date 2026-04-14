#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniGameReadyWidget.generated.h"

class UButton; 
class UTextBlock; 
class UImage;
class UMultiLineEditableText; 
class ADDMiniGameStateBase;

UCLASS()
class DOODOONG_API UMiniGameReadyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void OnReadyButtonClicked();
	
	UFUNCTION()
	void OnReadyStateChanged(int32 ReadyCount, int32 TotalCount); 
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MiniGameTitle;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ReadyStateText;
	
	UPROPERTY(meta = (BindWidget))
	UButton* ReadyButton;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ButtonText;
	
	UPROPERTY(meta = (BindWidget))
	UImage* MiniGameImage;
	
	UPROPERTY(meta = (BindWidget))
	UMultiLineEditableText* DescriptionText;
	
private:
	UPROPERTY()
	ADDMiniGameStateBase* CurrentGameState; 
};
