#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniGameBaseWidget.generated.h"


class ADDMiniGameStateBase;
class UHorizontalBox;
class UTextBlock;

UCLASS()
class DOODOONG_API UMiniGameBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
	virtual void CreatePlayerInfos(); 
	
	
	virtual void InitializePlayerInfoWidget(UUserWidget* PlayerInfoWidget);
	
protected:
	UFUNCTION()
	void OnRemainingTimeChanged(const float RemainingTime);
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RemainingTimeText; 
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> PlayerInfoContainer;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "UI")
	TSubclassOf<UUserWidget> PlayerInfoWidgetClass;
	
	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> PlayerInfoWidgets;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="UI")
	bool bShowRemainingTime = true; 
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float PlayerInfoWidth = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float PlayerInfoHeight = 120.f;
	
protected:
	UPROPERTY()
	ADDMiniGameStateBase* CachedMiniGameState; 
};
