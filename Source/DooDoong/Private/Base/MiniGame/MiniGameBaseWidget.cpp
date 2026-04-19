#include "Base/MiniGame/MiniGameBaseWidget.h"
#include "Base/MiniGame/DDMiniGameStateBase.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

void UMiniGameBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 바인드 
	CachedMiniGameState = GetWorld()->GetGameState<ADDMiniGameStateBase>();
	if (!CachedMiniGameState) return; 
	
	CachedMiniGameState->OnRemainingTimeChanged.AddDynamic(this, &ThisClass::OnRemainingTimeChanged);
}

void UMiniGameBaseWidget::NativeDestruct()
{
	if (CachedMiniGameState)
	{
		// 바인딩 해제 
		CachedMiniGameState->OnRemainingTimeChanged.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UMiniGameBaseWidget::CreatePlayerInfos()
{
	if (!CachedMiniGameState || !PlayerInfoContainer) return;
	
	PlayerInfoContainer->ClearChildren();
	for (auto PS : CachedMiniGameState->PlayerArray)
	{
		if (!PS) continue;
		
		// 위젯 생성 
		UUserWidget* InfoWidget = 
			CreateWidget<UUserWidget>(PlayerInfoContainer, PlayerInfoWidgetClass);
		
		// 초기화 ( 자식 클래스에서 정의 )  
		InitializePlayerInfoWidget(InfoWidget); 
		
		USizeBox* SizeBox = WidgetTree->ConstructWidget<USizeBox>();
		SizeBox->SetWidthOverride(PlayerInfoWidth);
		SizeBox->SetHeightOverride(PlayerInfoHeight);
		SizeBox->AddChild(InfoWidget);
		
		UHorizontalBoxSlot* BoxSlot = PlayerInfoContainer->AddChildToHorizontalBox(SizeBox);
		if (BoxSlot)
		{
			// 위젯 간 간격
			BoxSlot->SetPadding(FMargin(4.f, 0.f));
			BoxSlot->SetVerticalAlignment(VAlign_Center);
		}
		
		PlayerInfoWidgets.Add(InfoWidget);
	}
}

void UMiniGameBaseWidget::InitializePlayerInfoWidget(UUserWidget* PlayerInfoWidget)
{
}

void UMiniGameBaseWidget::OnRemainingTimeChanged(const float RemainingTime)
{
	FString TimeString = FString::Printf(TEXT("남은 시간 : %.2f"), RemainingTime);
	
	if (RemainingTimeText)
	{
		RemainingTimeText->SetText(FText::FromString(TimeString));
	}
}
