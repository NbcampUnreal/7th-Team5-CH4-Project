#include "UI/HUD/DDLobbyHUD.h"
#include "Blueprint/UserWidget.h"
#include "Base/Player/DDLobbyPlayerController.h"

void ADDLobbyHUD::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(LobbyWidgetClass))
	{
		LobbyWidgetInstance = CreateWidget<UUserWidget>(GetOwningPlayerController(), LobbyWidgetClass);
		if (IsValid(LobbyWidgetInstance))
		{
			LobbyWidgetInstance->AddToViewport();

			if (ADDLobbyPlayerController* LobbyPlayerController = Cast<ADDLobbyPlayerController>(GetOwningPlayerController()))
			{
				FInputModeUIOnly Mode;
				Mode.SetWidgetToFocus(LobbyWidgetInstance->GetCachedWidget());
				LobbyPlayerController->SetInputMode(Mode);
				LobbyPlayerController->bShowMouseCursor = true;
			}
		}
	}
}

void ADDLobbyHUD::HideLobbyUI()
{
	if (IsValid(LobbyWidgetInstance))
	{
		LobbyWidgetInstance->RemoveFromParent();
		LobbyWidgetInstance = nullptr;
	}
}
