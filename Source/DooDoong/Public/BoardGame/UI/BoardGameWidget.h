#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoardGameWidget.generated.h"

class ADDBoardGameState;
class UBoardGamePlayerInfo;
class UTextBlock;
class UVerticalBox; 
class USizeBox;
class ADDBasePlayerState; 

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
	void UpdateRemainingRound(int32 InRemainingRound); 
	
	void CreatePlayerInfoWidgets(const TArray<ADDBasePlayerState*>& PlayerStates); 
	
	UFUNCTION()
	void OnPlayerArrayChanged();
	
	void RefreshPlayerInfoWidgets(); 
	
	void SortPlayerInfoWidgets();
	
	void ClearPlayerInfoWidgets();
	
	UPROPERTY()
	TArray<TObjectPtr<USizeBox>> PlayerInfoSizeBoxes;

protected:
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UTextBlock> RemainTimeText; // 남은 시간 
	
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UTextBlock> RemainingRoundText; // 남은 라운드 

protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> PlayerInfoContainer; // 플레이어 정보들 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UBoardGamePlayerInfo> PlayerInfoWidgetClass; 
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float PlayerInfoWidth = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float PlayerInfoHeight = 120.f;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UBoardGamePlayerInfo>> PlayerInfoWidgets;
	
protected:
	
	UPROPERTY()
	TObjectPtr<ADDBoardGameState> CurrentGameState;
	
};
