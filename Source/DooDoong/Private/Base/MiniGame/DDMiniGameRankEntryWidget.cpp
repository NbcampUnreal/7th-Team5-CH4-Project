#include "Base/MiniGame/DDMiniGameRankEntryWidget.h"

#include "Base/Player/DDBasePlayerState.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "System/MiniGame/DDMiniGameDefinition.h"

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
		const AGameStateBase* GameState = GetWorld() != nullptr ? GetWorld()->GetGameState() : nullptr;
		if (GameState != nullptr)
		{
			for (APlayerState* PlayerState : GameState->PlayerArray)
			{
				if (PlayerState == nullptr || PlayerState->GetPlayerId() != Entry.PlayerId)
				{
					continue;
				}

				if (const ADDBasePlayerState* PS = Cast<ADDBasePlayerState>(PlayerState))
				{
					NameText->SetColorAndOpacity(PS->PlayerGameData.PlayerColor);
				}
				break;
			}
		}
	}
}
