#include "System/DDUIManagerSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Data/DDUIConfig.h"
#include "Engine/LocalPlayer.h"

void UDDUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UDDUIManagerSubsystem::Deinitialize()
{
	HideOverlay();
	HidePopup();
	
	if (GameLayerWidget) GameLayerWidget->RemoveFromParent();
	
	Super::Deinitialize();
}

void UDDUIManagerSubsystem::SwitchGameLayer(FGameplayTag StateTag)
{
	if (!UIConfig) return;

	// 기존 위젯 제거
	if (GameLayerWidget)
	{
		GameLayerWidget->RemoveFromParent();
		GameLayerWidget = nullptr;
	}

	// StateWidgetMap에서 클래스 찾기
	TSubclassOf<UUserWidget>* WidgetClass = UIConfig->StateWidgetMap.Find(StateTag);
	if (!WidgetClass) return;

	GameLayerWidget = CreateUIWidget(*WidgetClass);
	if (GameLayerWidget)
	{
		GameLayerWidget->AddToViewport(0);
	}
}

void UDDUIManagerSubsystem::ShowOverlay(TSubclassOf<UUserWidget> WidgetClass)
{
	if (OverlayWidget)
	{
		OverlayWidget->RemoveFromParent();
		OverlayWidget = nullptr;
	}

	OverlayWidget = CreateUIWidget(WidgetClass);
	if (OverlayWidget)
		OverlayWidget->AddToViewport(5);
}

void UDDUIManagerSubsystem::HideOverlay()
{
	if (OverlayWidget)
	{
		OverlayWidget->RemoveFromParent();
		OverlayWidget = nullptr;
	}
}

void UDDUIManagerSubsystem::TogglePopup(FGameplayTag PopupTag)
{
	if (!UIConfig) return;
	
	if (PopupWidget)
	{
		PopupWidget->RemoveFromParent();
		PopupWidget = nullptr;
		return;
	}
	
	TSubclassOf<UUserWidget>* WidgetClass = UIConfig->PopupWidgetMap.Find(PopupTag);
	if (!WidgetClass) return;

	PopupWidget = CreateUIWidget(*WidgetClass);
	if (PopupWidget) PopupWidget->AddToViewport(10);
}

void UDDUIManagerSubsystem::HidePopup()
{
	if (PopupWidget)
	{
		PopupWidget->RemoveFromParent();
		PopupWidget = nullptr;
	}
}

void UDDUIManagerSubsystem::SetUIConfig(UDDUIConfig* InUIConfig)
{
	if (!InUIConfig) return;
	
	UIConfig = InUIConfig;
	
	if (UIConfig->DefaultOverlayClass)
	{
		ShowOverlay(UIConfig->DefaultOverlayClass);
	}
}

UUserWidget* UDDUIManagerSubsystem::CreateUIWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (WidgetClass == nullptr) return nullptr;
	
	APlayerController* PC = GetLocalPlayer()->GetPlayerController(GetWorld());
	if (!PC) return nullptr;

	return CreateWidget<UUserWidget>(PC, WidgetClass);
}
