#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDStartCountWidget.generated.h"

class ADDLobbyGameState;
class UTextBlock;

UCLASS()
class DOODOONG_API UDDStartCountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
protected:
	
	UFUNCTION()
	void UpdateCount(int32 inCount);
	
protected:
	UPROPERTY(meta =(BindWidget))
	UTextBlock* CountText;
	
	UPROPERTY()
	TObjectPtr<ADDLobbyGameState> CurrentGameState;
};
