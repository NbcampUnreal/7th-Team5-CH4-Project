#include "System/DDUIManagerSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Common/DDLogManager.h"
#include "Data/DDUIConfig.h"
#include "Engine/LocalPlayer.h"
#include "Lobby/UI/DDErrorMessageWidget.h"

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
	if (!UIConfig)
	{
		return;
	}
	
	if (PopupWidgets.Contains(PopupTag))
	{
		return;
	}

	TSubclassOf<UUserWidget>* WidgetClass = UIConfig->PopupWidgetMap.Find(PopupTag);
	if (!WidgetClass || !*WidgetClass)
	{
		return;
	}
	
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
		if (IsValid(Widget->Get()))
		{
			Widget->Get()->RemoveFromParent();
		}
		PopupWidgets.Remove(PopupTag);
	}
}

void UDDUIManagerSubsystem::HideAllPopups()
{
	for (auto& Pair : PopupWidgets)
	{
		if (IsValid(Pair.Value))
		{
			Pair.Value->RemoveFromParent();
		}
	}
	
	PopupWidgets.Empty();
}

void UDDUIManagerSubsystem::SetUIConfig(UDDUIConfig* InUIConfig)
{
	if (!InUIConfig) return;
	
	HideAllPopups();
	UIConfig = InUIConfig;
	
	if (UIConfig->DefaultOverlayClass)
	{
		ShowOverlay(UIConfig->DefaultOverlayClass);
	}
}

void UDDUIManagerSubsystem::DrawErrorMessage(const FString& ErrorMessage, float Duration)
{
	if (!UIConfig || !UIConfig->ErrorMessageWidget) return;
	
	// 에러 메시지 위젯 생성 
	UDDErrorMessageWidget* ErrorMessageWidget = 
		Cast<UDDErrorMessageWidget>(CreateUIWidget(UIConfig->ErrorMessageWidget));
	if (!ErrorMessageWidget)
	{
		LOG_KMS(Warning, TEXT("[UI Manager] : Fail to Draw Error Message"));
		return; 
	}
	
	// 에러 메시지 입력 
	LOG_KMS(Warning, TEXT("[UI Manager] : Draw Error Message"));
	ErrorMessageWidget->UpdateErrorMessage(ErrorMessage);
	
	ErrorMessageWidget->AddToViewport(50); 
	
	// 위젯 제거 타이머 시작 
	FTimerHandle ErrorTimerHandle;
	TWeakObjectPtr<UUserWidget> WeakWidget = ErrorMessageWidget;
	GetWorld()->GetTimerManager().SetTimer(
		ErrorTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [WeakWidget]()
		{
			if (WeakWidget.IsValid()) WeakWidget->RemoveFromParent();
			
			LOG_KMS(Warning, TEXT("[UI Manager] : Remove Error Message"));
		}),
		Duration,
		false
	);
}

UUserWidget* UDDUIManagerSubsystem::CreateUIWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (WidgetClass == nullptr) return nullptr;
	
	APlayerController* PC = GetLocalPlayer()->GetPlayerController(GetWorld());
	if (!PC) return nullptr;

	return CreateWidget<UUserWidget>(PC, WidgetClass);
}
