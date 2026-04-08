#include "UI/HUD/MiniGameReadyHUD.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

void AMiniGameReadyHUD::ShowReadyUI()
{
    if (!ReadyWidgetClass) return;
    if (ReadyWidget) return;

    APlayerController* PC = GetOwningPlayerController();
    if (!PC) return;

    ReadyWidget = CreateWidget<UUserWidget>(PC, ReadyWidgetClass);
    if (ReadyWidget)
    {
        ReadyWidget->AddToViewport();

        FInputModeUIOnly Mode;
        Mode.SetWidgetToFocus(ReadyWidget->TakeWidget());
        PC->SetInputMode(Mode);
        PC->bShowMouseCursor = true;
    }
}

void AMiniGameReadyHUD::HideReadyUI()
{
    if (!ReadyWidget) return;

    ReadyWidget->RemoveFromParent();
    ReadyWidget = nullptr;

    APlayerController* PC = GetOwningPlayerController();
    if (PC)
    {
        FInputModeGameOnly Mode;
        PC->SetInputMode(Mode);
        PC->bShowMouseCursor = false;
    }
}