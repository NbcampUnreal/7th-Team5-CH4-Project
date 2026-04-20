#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FinalResultWidget.generated.h"

class UTextBlock;
class UVerticalBox;
class UBoardGamePlayerInfo; 
class UButton; 

UCLASS()
class DOODOONG_API UFinalResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
protected:
	void CreatePlayerInfoWidgets(const TArray<ADDBasePlayerState*>& WidgetInfos);
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> PlayerInfoContainer; // 플레이어 정보들 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UBoardGamePlayerInfo> PlayerInfoWidgetClass; 
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float PlayerInfoWidth = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float PlayerInfoHeight = 120.f;
};
