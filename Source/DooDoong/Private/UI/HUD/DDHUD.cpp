#include "UI/HUD/DDHUD.h"
#include "Base/Game/DDGameStateBase.h"
#include "Blueprint/UserWidget.h"
#include "Common/DDLogManager.h"

void ADDHUD::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeLevelUI();
}

void ADDHUD::InitializeLevelUI()
{
	// 1. 기존 위젯이 있으면 제거 
	if (CurrentMainWidgetInst)
	{
		CurrentMainWidgetInst->RemoveFromParent();
		CurrentMainWidgetInst = nullptr;
	}
	
	// 2. 현재 맵의 GameState에서 위젯 클래스 가져와서 생성 
	ADDGameStateBase* GS = GetWorld()->GetGameState<ADDGameStateBase>();
	if (!IsValid(GS)) return; 
	
	if (IsValid(GS->LevelMainWidgetClass))
	{
		CurrentMainWidgetInst = CreateWidget<UUserWidget>(GetOwningPlayerController(), GS->LevelMainWidgetClass);
		if (CurrentMainWidgetInst)
		{
			CurrentMainWidgetInst->AddToViewport();
			LOG_KMS(Warning, TEXT("[HUD] : Successfully Created Level UI"));
		}
	}
}

void ADDHUD::ToggleWidgetVisibility(bool bVisible)
{
	if (IsValid(CurrentMainWidgetInst))
	{
		CurrentMainWidgetInst->SetVisibility(
			bVisible ? 	
			ESlateVisibility::Visible :
			ESlateVisibility::Hidden
		);
	}
}

