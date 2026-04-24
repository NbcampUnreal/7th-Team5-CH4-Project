#include "BoardGame/UI/FinalResultWidget.h"

#include "Blueprint/WidgetTree.h"
#include "BoardGame/Game/DDBoardGameState.h"
#include "BoardGame/UI/FinalResultPlayerInfo.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UFinalResultWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);
	}
}

void UFinalResultWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	CachedBoardGameState = GetWorld()->GetGameState<ADDBoardGameState>();
	if (!CachedBoardGameState) return; 
	
	const TArray<FFinalRankData>& Rankings = CachedBoardGameState->GetFinalRankings();
	if (Rankings.Num() > 0)
	{
		CreatePlayerInfoWidgets(Rankings);
	}
	
	CachedBoardGameState->OnFinalRankingsUpdated.AddDynamic(this, &ThisClass::OnFinalRankingsReceived);
}

void UFinalResultWidget::NativeDestruct()
{
	if (CachedBoardGameState)
	{
		CachedBoardGameState->OnFinalRankingsUpdated.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UFinalResultWidget::CreatePlayerInfoWidgets(const TArray<FFinalRankData>& Rankings)
{
	if (!PlayerInfoContainer || !PlayerInfoWidgetClass) return;

	PlayerInfoContainer->ClearChildren();

	for (const FFinalRankData& RankData : Rankings)
	{
		UFinalResultPlayerInfo* InfoWidget =
			CreateWidget<UFinalResultPlayerInfo>(GetOwningPlayer(), PlayerInfoWidgetClass);
		if (!InfoWidget) continue;

		InfoWidget->InitializePlayerInfo(RankData);
		
		USizeBox* SizeBox = WidgetTree->ConstructWidget<USizeBox>();
		SizeBox->SetWidthOverride(PlayerInfoWidth);
		SizeBox->SetHeightOverride(PlayerInfoHeight);
		SizeBox->AddChild(InfoWidget);

		if (UVerticalBoxSlot* BoxSlot = PlayerInfoContainer->AddChildToVerticalBox(SizeBox))
		{
			BoxSlot->SetPadding(FMargin(0.f, 4.f));
			BoxSlot->SetHorizontalAlignment(HAlign_Fill);
		}
	}
}

void UFinalResultWidget::OnFinalRankingsReceived(const TArray<FFinalRankData>& Rankings)
{
	if (PlayerInfoContainer && PlayerInfoContainer->GetChildrenCount() > 0) return;

	CreatePlayerInfoWidgets(Rankings);
}


void UFinalResultWidget::OnCloseButtonClicked()
{
	RemoveFromParent();
}
