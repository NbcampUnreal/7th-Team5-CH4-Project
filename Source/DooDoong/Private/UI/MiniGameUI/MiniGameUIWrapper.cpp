#include "UI/MiniGameUI/MiniGameUIWrapper.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Input/Reply.h"

void UMiniGameUIWrapper::OpenReadyUI(APlayerController* PC)
{
    if (!ReadyWidgetClass || !PC) return;

    if (!ReadyWidget)
    {
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
}

void UMiniGameUIWrapper::CloseReadyUI(APlayerController* PC)
{
    if (ReadyWidget)
    {
        ReadyWidget->RemoveFromParent();
        ReadyWidget = nullptr;
    }

    if (PC)
    {
        FInputModeGameOnly Mode;
        PC->SetInputMode(Mode);
        PC->bShowMouseCursor = false;
    }
}