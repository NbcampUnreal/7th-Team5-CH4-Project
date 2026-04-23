#include "BoardGame/UI/FinalResultPlayerInfo.h"
#include "Components/TextBlock.h"
#include "BoardGame/Game/DDBoardGameState.h"

void UFinalResultPlayerInfo::InitializePlayerInfo(const FFinalRankData& RankData)
{
	if (RankText)
	{
		int32 Rank = RankData.Rank; 
		FString RankString = 
			(Rank == 1) ? FString("1st") :
			(Rank == 2) ? FString("2nd") :
			(Rank == 3) ? FString("3rd") :
			FString::Printf(TEXT("%dth"), Rank);
		
		RankText->SetText(FText::FromString(RankString));
		
		if (RankColors.Num() > 0)
		{
			FLinearColor RankColor = (RankColors.Num() < Rank) ? 
				RankColors[0] : RankColors[Rank % RankColors.Num()];
			
			RankText->SetColorAndOpacity(RankColor);
		}
	}
	
	if (PlayerNameText)
	{
		FName PlayerNameString = RankData.PlayerName; 
		PlayerNameText->SetText(FText::FromName(PlayerNameString));
	}
	
	if (TrophyText)
	{
		FString TrophyString = FString::Printf(TEXT("트로피 : %d 개"), RankData.Trophy);
		TrophyText->SetText(FText::FromString(TrophyString));
	}
	
	if (CoinText)
	{
		FString CoinString = FString::Printf(TEXT("코인 : %d 개"), RankData.Coin);
		CoinText->SetText(FText::FromString(CoinString));
	}
}
