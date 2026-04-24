#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FinalResultWidget.generated.h"

struct FFinalRankData;
class ADDBoardGameState;
class UTextBlock;
class UVerticalBox;
class UFinalResultPlayerInfo; 
class UButton; 
class ADDBasePlayerState;

UCLASS()
class DOODOONG_API UFinalResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
protected:
	void CreatePlayerInfoWidgets(const TArray<FFinalRankData>& Rankings);
	
	UFUNCTION()
	void OnFinalRankingsReceived(const TArray<FFinalRankData>& Rankings);
	
	UFUNCTION()
	void OnCloseButtonClicked();
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> PlayerInfoContainer; // 플레이어 정보들 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UFinalResultPlayerInfo> PlayerInfoWidgetClass; 
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float PlayerInfoWidth = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float PlayerInfoHeight = 120.f;
	
private:
	UPROPERTY()
	TObjectPtr<ADDBoardGameState> CachedBoardGameState; 
};
