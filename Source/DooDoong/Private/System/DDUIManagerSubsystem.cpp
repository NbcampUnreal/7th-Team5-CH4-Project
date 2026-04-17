#include "System/DDUIManagerSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Common/DDLogManager.h"
#include "Data/DDUIConfig.h"
#include "Engine/LocalPlayer.h"

void UDDUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UDDUIManagerSubsystem::Deinitialize()
{
	HideOverlay();
	HideAllPopups();
	
	if (GameLayerWidget)
	{
		GameLayerWidget->RemoveFromParent();
		GameLayerWidget = nullptr;
	}
	
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

void UDDUIManagerSubsystem::DrawPopup(FGameplayTag PopupTag)
{
	if (!UIConfig) return;
	
	if (PopupWidgets.Contains(PopupTag)) return;

	TSubclassOf<UUserWidget>* WidgetClass = UIConfig->PopupWidgetMap.Find(PopupTag);
	if (!WidgetClass) return;
	
	UUserWidget* NewWidget = CreateUIWidget(*WidgetClass);
	if (NewWidget)
	{
		NewWidget->AddToViewport(10);
		PopupWidgets.Add(PopupTag, NewWidget);
		LOG_KMS(Warning, TEXT("[UI Manager] : Open New Popup Widget"));
	}
}

void UDDUIManagerSubsystem::DrawPopupForDuration(FGameplayTag PopupTag, float Duration)
{
	DrawPopup(PopupTag); 
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateUObject(this, &UDDUIManagerSubsystem::HidePopup, PopupTag),
		Duration,
		false
	);
}

void UDDUIManagerSubsystem::HidePopup(FGameplayTag PopupTag)
{
	if (TObjectPtr<UUserWidget>* Widget = PopupWidgets.Find(PopupTag))
	{
		(*Widget)->RemoveFromParent();
		PopupWidgets.Remove(PopupTag);
	}
	
	TSubclassOf<UUserWidget>* WidgetClass = UIConfig->PopupWidgetMap.Find(PopupTag);
	if (!WidgetClass) return;
	
	UUserWidget* NewWidget = CreateUIWidget(*WidgetClass);
	if (NewWidget)
	{
		NewWidget->AddToViewport(10);
		PopupWidgets.Add(PopupTag, NewWidget);
		LOG_KMS(Warning, TEXT("[UI Manager] : Open New Popup Widget"));
	}
	
}

void UDDUIManagerSubsystem::HideAllPopups()
{
	for (auto& Pair : PopupWidgets)
	{
		if (Pair.Value)
			Pair.Value->RemoveFromParent();
	}
	
	PopupWidgets.Empty();
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
