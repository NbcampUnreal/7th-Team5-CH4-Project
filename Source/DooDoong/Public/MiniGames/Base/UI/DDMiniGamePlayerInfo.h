#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDMiniGamePlayerInfo.generated.h"

class ADDMiniGameStateBase;
class ADDBasePlayerState;
class UTextBlock; 

UCLASS()
class DOODOONG_API UDDMiniGamePlayerInfo : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	void InitializePlayerInfo(APlayerState* InPlayerState); 
	
public:
	UFUNCTION()
	void UpdateScore(int32 InScore); 

	UFUNCTION()
	void OnScoreBoardChanged();
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameText; 
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreText; 
	
private:
	TObjectPtr<ADDMiniGameStateBase> CachedGameState;
	
	TObjectPtr<ADDBasePlayerState> CachedPlayerState;
};
