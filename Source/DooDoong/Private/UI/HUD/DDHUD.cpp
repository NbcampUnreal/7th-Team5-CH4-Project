#include "UI/HUD/DDHUD.h"
#include "Blueprint/UserWidget.h"

void ADDHUD::BeginPlay()
{
	Super::BeginPlay();

	// Widget 클래스 없으면 종료
	if (!IsValid(MainWidgetClass)) return;

	APlayerController* PC = GetOwningPlayerController();
	if (!IsValid(PC)) return;

	MainWidgetInstance = CreateWidget<UUserWidget>(PC, MainWidgetClass);

	if (IsValid(MainWidgetInstance))
	{
		MainWidgetInstance->AddToViewport();

		// 필요하면 UI 모드 (선택)
		FInputModeGameAndUI Mode;
		Mode.SetWidgetToFocus(MainWidgetInstance->GetCachedWidget());
		PC->SetInputMode(Mode);
		PC->bShowMouseCursor = true;
	}
}