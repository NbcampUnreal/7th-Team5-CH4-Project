#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDLobbyWidget.generated.h"

class UButton;
class UTextBlock; 
class ADDLobbyGameState;

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

protected:
	
	UFUNCTION()
	void UpdatePlayerCount(int32 NewCount); 
	
	UFUNCTION()
	void UpdateReadyCount(int32 NewCount); 
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = "UI|Style")
	FButtonStyle NormalStyle;  

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = "UI|Style")
	FButtonStyle ReadyStyle; 
	
	bool bIsReady = false; 
	
	int32 PlayerCount = 0;
	
	int32 ReadyCount = 0; 
	
protected:
	
	UPROPERTY()
	TObjectPtr<ADDLobbyGameState> CurrentGameState;
};
