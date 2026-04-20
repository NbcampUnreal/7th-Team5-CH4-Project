#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoardGamePlayerInfo.generated.h"

class UTextBlock; 
class ADDBasePlayerState;

UCLASS()
class DOODOONG_API UBoardGamePlayerInfo : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	void InitializePlayerInfo(APlayerState* PlayerState);
	
	virtual void NativeDestruct() override;
	
protected:
	UFUNCTION()
	void UpdateHealth(int32 Cur, int32 Max);
	
	UFUNCTION()
	void UpdateCoinCount(int32 Coin);
	
	UFUNCTION()
	void UpdateTrophyCount(int32 Trophy);
	
	UFUNCTION()
	void UpdateCurrentRank(int32 CurrentRank);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameText; 
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TrophyText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CoinText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentRankText;
	
	UPROPERTY()
	FLinearColor PlayerColor = FLinearColor::White;
	
protected:
	UPROPERTY()
	TObjectPtr<ADDBasePlayerState> CurrentPlayerState;
	
};
