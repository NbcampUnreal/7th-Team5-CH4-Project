#include "Base/MiniGame/MiniGameResultWidget.h"
#include "Base/MiniGame/DDMiniGameRankEntryWidget.h"
#include "Base/MiniGame/DDMiniGameStateBase.h"
#include "Common/DDLogManager.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "System/DDGameplayTags.h"

void UMiniGameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	CachedMiniGameState = GetWorld()->GetGameState<ADDMiniGameStateBase>();
	if (!CachedMiniGameState)
	{
		LOG_KMS(Warning, TEXT("[UMiniGameResultWidget] : GS is null. "))
		return; 
	}
	
	CachedMiniGameState->OnMiniGameStateTagChanged.AddDynamic(this, &ThisClass::OnMiniGameStateChanged);

	InitializeResult(CachedMiniGameState->GetScoreBoard());
}

void UMiniGameResultWidget::NativeDestruct()
{
	if (CachedMiniGameState)
	{
		CachedMiniGameState->OnMiniGameStateTagChanged.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UMiniGameResultWidget::InitializeResult(const TArray<FMiniGameScoreEntry>& ScoreBoard)
{
	LOG_KMS(Warning, TEXT("[UMiniGameResultWidget] :InitializeResult."))
	if (ScoreBoard.IsEmpty()) return;
	
	// Rank 오름차순 정렬
	TArray<FMiniGameScoreEntry> Sorted = ScoreBoard;
	Sorted.Sort([](const FMiniGameScoreEntry& A, const FMiniGameScoreEntry& B)
	{
		return A.Rank < B.Rank;
	});

	// 1등 표시
	if (WinnerNameText)
	{
		FName WinnerName = Sorted[0].DisplayName;
		WinnerNameText->SetText(FText::FromName(WinnerName));
	}

	PopulateRankEntries(Sorted);
}

void UMiniGameResultWidget::OnMiniGameStateChanged(FGameplayTag StateTag)
{
	if (StateTag == DDGameplayTags::State_MiniGame_Completed && CachedMiniGameState)
	{
		InitializeResult(CachedMiniGameState->GetScoreBoard());
	}
}

void UMiniGameResultWidget::PopulateRankEntries(const TArray<FMiniGameScoreEntry>& Sorted)
{
	if (!PlayerNameContainer || !IsValid(RankEntryWidgetClass)) return;
	
	PlayerNameContainer->ClearChildren();

	for (int32 i = 1; i < Sorted.Num(); ++i)
	{
		UDDMiniGameRankEntryWidget* EntryWidget = CreateWidget<UDDMiniGameRankEntryWidget>(
			GetOwningPlayer(), RankEntryWidgetClass);

		if (!EntryWidget) continue;

		EntryWidget->SetEntryData(Sorted[i]);
		
		if (UHorizontalBoxSlot* BoxSlot = PlayerNameContainer->AddChildToHorizontalBox(EntryWidget))
		{
			BoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill)); 
			BoxSlot->SetHorizontalAlignment(HAlign_Center);
			BoxSlot->SetVerticalAlignment(VAlign_Center);
		}
	}
}
