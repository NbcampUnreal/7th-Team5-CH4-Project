#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDErrorMessageWidget.generated.h"

class UTextBlock; 

UCLASS()
class DOODOONG_API UDDErrorMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void UpdateErrorMessage(const FString& InErrorMessage); 
	
protected:
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ErrorMessageText;
};
