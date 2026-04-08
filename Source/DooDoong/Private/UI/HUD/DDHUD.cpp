#include "UI/HUD/DDHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ADDHUD::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PC = GetOwningPlayerController();
    if (!IsValid(PC)) return;

    FString MapName = UGameplayStatics::GetCurrentLevelName(this);

    TSubclassOf<UUserWidget> WidgetToCreate = nullptr;

    // 로비
    if (MapName.Contains("L_Lobby"))
    {
        WidgetToCreate = LobbyWidgetClass;
    }
    // 기본 게임
    else if (MapName.Contains("L_BaseGame"))
    {
        WidgetToCreate = BaseGameWidgetClass;
    }
    // ❗ 미니게임 (Platformer 포함)
    else if (MapName.Contains("L_Platformer"))
    {
        WidgetToCreate = nullptr; // 🔥 일부러 생성 안 함
    }

    ShowWidget(WidgetToCreate);
}

void ADDHUD::ShowWidget(TSubclassOf<UUserWidget> WidgetClass)
{
    if (!IsValid(WidgetClass)) return;

    APlayerController* PC = GetOwningPlayerController();
    if (!IsValid(PC)) return;

    HideMainWidget();

    MainWidgetInstance = CreateWidget<UUserWidget>(PC, WidgetClass);

    if (MainWidgetInstance)
    {
        MainWidgetInstance->AddToViewport();

        FInputModeGameAndUI Mode;
        Mode.SetWidgetToFocus(MainWidgetInstance->GetCachedWidget());

        PC->SetInputMode(Mode);
        PC->bShowMouseCursor = true;
    }
}

void ADDHUD::HideMainWidget()
{
    if (IsValid(MainWidgetInstance))
    {
        MainWidgetInstance->RemoveFromParent();
        MainWidgetInstance = nullptr;
    }
}