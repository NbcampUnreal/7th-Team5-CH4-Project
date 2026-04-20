#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniGameResultWidget.generated.h"

UCLASS()
class DOODOONG_API UMiniGameResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
protected:
};
