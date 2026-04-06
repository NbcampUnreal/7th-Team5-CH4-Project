#include "UI/HUD/DDHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ADDHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetOwningPlayerController();
	if (!IsValid(PC)) return;

	// 현재 맵 이름 가져오기
	FString MapName = UGameplayStatics::GetCurrentLevelName(this);

	TSubclassOf<UUserWidget> WidgetToCreate = nullptr;

	// 맵 기준 분기
	if (MapName.Contains("L_Lobby"))
	{
		WidgetToCreate = LobbyWidgetClass;
	}
	else if (MapName.Contains("L_BaseGame"))
	{
		WidgetToCreate = BaseGameWidgetClass;
	}

	// 생성
	ShowWidget(WidgetToCreate);
}

void ADDHUD::ShowWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (!IsValid(WidgetClass)) return;

	APlayerController* PC = GetOwningPlayerController();
	if (!IsValid(PC)) return;

	// 기존 UI 제거
	HideMainWidget();

	// 새 UI 생성
	MainWidgetInstance = CreateWidget<UUserWidget>(PC, WidgetClass);

	if (MainWidgetInstance)
	{
		MainWidgetInstance->AddToViewport();

		// 입력 모드 (필요할 때만)
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

