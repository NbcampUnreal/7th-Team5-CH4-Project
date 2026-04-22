#include "Base/MiniGame/MiniGameBaseWidget.h"
#include "Base/MiniGame/DDMiniGameStateBase.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "MiniGames/Base/UI/DDMiniGamePlayerInfo.h"

void UMiniGameBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 바인드 
	CachedMiniGameState = GetWorld()->GetGameState<ADDMiniGameStateBase>();
	if (!CachedMiniGameState) return; 
	
	CachedMiniGameState->OnRemainingTimeChanged.AddDynamic(
		this, &ThisClass::OnRemainingTimeChanged);
	
	CreatePlayerInfos(); 
}

void UMiniGameBaseWidget::NativeDestruct()
{
	// 바인딩 해제 
	if (CachedMiniGameState)
	{
		CachedMiniGameState->OnRemainingTimeChanged.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UMiniGameBaseWidget::CreatePlayerInfos()
{
	if (!CachedMiniGameState || !PlayerInfoContainer || !PlayerInfoWidgetClass) return;
	
	PlayerInfoContainer->ClearChildren();
	PlayerInfoWidgets.Empty();
	

	
	for (auto PS : CachedMiniGameState->PlayerArray)
	{
		if (!PS) continue;
		
		// 위젯 생성 
		UDDMiniGamePlayerInfo* InfoWidget = 
			CreateWidget<UDDMiniGamePlayerInfo>(PlayerInfoContainer, PlayerInfoWidgetClass);
		if (!InfoWidget) continue;
		
		// 초기화 ( 자식 클래스에서 정의 )  
		InitializePlayerInfoWidget(InfoWidget); 
		
		USizeBox* SizeBox = WidgetTree->ConstructWidget<USizeBox>();
		SizeBox->SetWidthOverride(PlayerInfoWidth);
		SizeBox->SetHeightOverride(PlayerInfoHeight);
		SizeBox->AddChild(InfoWidget);
		
		if (UHorizontalBoxSlot* SlotBox = PlayerInfoContainer->AddChildToHorizontalBox(InfoWidget))
		{
			SlotBox->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			SlotBox->SetPadding(FMargin(4.f, 0.f));
			SlotBox->SetHorizontalAlignment(HAlign_Fill);
			SlotBox->SetVerticalAlignment(VAlign_Fill);
		}
		
		PlayerInfoWidgets.Add(InfoWidget);
	}
}

void UMiniGameBaseWidget::OnRemainingTimeChanged(const float RemainingTime)
{
	FString TimeString = FString::Printf(TEXT("남은 시간 : %.2f"), RemainingTime);
	
	if (RemainingTimeText)
	{
		RemainingTimeText->SetText(FText::FromString(TimeString));
	}
}


