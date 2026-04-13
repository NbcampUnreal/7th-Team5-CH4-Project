#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DDHUD.generated.h"

class UUserWidget;

UCLASS()
class DOODOONG_API ADDHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
public:
	void InitializeLevelUI();
	
	void ToggleWidgetVisibility(bool bVisible);

public:
	template<typename T>
	T* GetCurrentWidget() const 
	{ 
		return Cast<T>(CurrentMainWidgetInst); 
	}
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> CurrentMainWidgetClass;
	
	UPROPERTY()
	UUserWidget* CurrentMainWidgetInst;
};

