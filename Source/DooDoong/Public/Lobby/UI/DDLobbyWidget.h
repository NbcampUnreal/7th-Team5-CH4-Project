#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDLobbyWidget.generated.h"

class UButton;
class UTextBlock; 

UCLASS()
class DOODOONG_API UDDLobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
protected:
	
	UFUNCTION()
	void OnClickReadButton(); 

	UFUNCTION()
	void OnClickQuitButton(); 
	
	UFUNCTION()
	void UpdatePlayerCount(); 
	
	UFUNCTION()
	void UpdateReadyCount(); 

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerCountText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReadyButton; 
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ReadyText; 
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ReadyCountText; 
	
	UPROPERTY(EditDefaultsOnly, Category = "UI|Style")
	FButtonStyle NormalStyle;  

	UPROPERTY(EditDefaultsOnly, Category = "UI|Style")
	FButtonStyle ReadyStyle; 
	
	bool bIsReady = false; 
	
protected:
	
	// UPROPERTY(meta = (BindWidget))
	// TObjectPtr<UDDLobbyGameState> CurrentGameState;
};
