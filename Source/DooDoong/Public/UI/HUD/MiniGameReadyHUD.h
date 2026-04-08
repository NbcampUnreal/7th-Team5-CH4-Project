#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MiniGameReadyHUD.generated.h"

class UUserWidget;

UCLASS()
class DOODOONG_API AMiniGameReadyHUD : public AHUD
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> ReadyWidgetClass;

private:
    UPROPERTY()
    UUserWidget* ReadyWidget = nullptr;

public:
    void ShowReadyUI();
    void HideReadyUI();
};