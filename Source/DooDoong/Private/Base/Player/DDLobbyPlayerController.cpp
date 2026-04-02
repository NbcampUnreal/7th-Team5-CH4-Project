#include "Base/Player/DDLobbyPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Base/Game/DDLobbyGameMode.h"

void ADDLobbyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController() && IsValid(UIWidgetClass))
    {
        UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
        if (IsValid(UIWidgetInstance))
        {
            UIWidgetInstance->AddToViewport();

            // UI가 떠 있는 동안 캐릭터 조작 차단 (관찰 허용)
            FInputModeUIOnly Mode;
            Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
            SetInputMode(Mode);
            bShowMouseCursor = true;
        }
    }
}

bool ADDLobbyPlayerController::Server_SubmitNickname_Validate(const FString& InNickname)
{
    return true;
}

void ADDLobbyPlayerController::Server_SubmitNickname_Implementation(const FString& InNickname)
{
    ADDLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ADDLobbyGameMode>();
    if (IsValid(LobbyGameMode))
    {
        LobbyGameMode->ProcessPlayerJoin(this, InNickname);
    }
}

void ADDLobbyPlayerController::Client_JoinLobby_Implementation()
{
    if (IsValid(UIWidgetInstance))
    {
        UIWidgetInstance->RemoveFromParent();
    }

    // 닉네임 입력 완료 후 캐릭터 조작 권한 활성화
    FInputModeGameOnly Mode;
    SetInputMode(Mode);
    bShowMouseCursor = false;
}