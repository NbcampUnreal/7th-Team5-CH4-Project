#include "Base/MiniGame/DDMiniGameRankEntryWidget.h"
#include "System/MiniGame/DDMiniGameDefinition.h"
#include "Components/TextBlock.h"

void UDDMiniGameRankEntryWidget::SetEntryData(const FMiniGameScoreEntry& Entry)
{
	if (RankText)
	{  
		FString RankString = 
			(Entry.Rank == 1) ? FString::Printf(TEXT("1st")) :
			(Entry.Rank == 2) ? FString::Printf(TEXT("2nd")) :
			(Entry.Rank == 3) ? FString::Printf(TEXT("3rd")) :
			 FString::Printf(TEXT("%dth"), Entry.Rank); 
		
		RankText->SetText(FText::FromString(RankString));
	}
	
	if (NameText)
	{
		NameText->SetText(FText::FromName(Entry.DisplayName));
		ADDBasePlayerState* PS = Cast<ADDBasePlayerState>(Entry.PlayerState);
		if (PS)
		{
			NameText->SetColorAndOpacity(PS->PlayerGameData.PlayerColor);
		}
	}
}
