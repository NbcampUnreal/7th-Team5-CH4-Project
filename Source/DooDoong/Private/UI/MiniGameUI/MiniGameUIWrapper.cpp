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


void UMiniGameUIWrapper::UpdateReadyCount(int32 ReadyCount, int32 TotalCount)
{
    if (!ReadyWidget) return;

    // 방법 1 (추천): 위젯에 함수 만들어서 호출
    // IReadyInterface 같은 거 써도 되고

    // 예시 (블루프린트 이벤트 호출)
    UFunction* Func = ReadyWidget->FindFunction(FName("UpdateReadyText"));
    if (Func)
    {
        struct FParams
        {
            int32 Ready;
            int32 Total;
        };

        FParams Params;
        Params.Ready = ReadyCount;
        Params.Total = TotalCount;

        ReadyWidget->ProcessEvent(Func, &Params);
    }
}