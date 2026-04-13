#include "UI/GameUIManager.h"
#include "UI/UIConfigData.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

void UGameUIManager::Initialize(APlayerController* InPC, UUIConfigData* InConfig)
{
	PC = InPC;
	UIConfig = InConfig;
}


void UGameUIManager::ShowMiniGameReadyUI()
{
	if (!PC || !UIConfig || !UIConfig->MiniGameReadyWidget)
		return;

	if (!ReadyWidget)
	{
		ReadyWidget = CreateWidget<UUserWidget>(PC, UIConfig->MiniGameReadyWidget);
	}

	if (ReadyWidget && !ReadyWidget->IsInViewport())
	{
		ReadyWidget->AddToViewport();
	}
}


void UGameUIManager::HideMiniGameReadyUI()
{
	if (ReadyWidget)
	{
		ReadyWidget->RemoveFromParent();
	}
}