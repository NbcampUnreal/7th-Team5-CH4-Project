#include "MiniGames/Base/UI/DDMiniGamePlayerInfo.h"

#include "Base/MiniGame/DDMiniGameStateBase.h"
#include "Base/Player/DDBasePlayerState.h"
#include "Components/TextBlock.h"

void UDDMiniGamePlayerInfo::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDDMiniGamePlayerInfo::NativeDestruct()
{
	if (CachedGameState)
		CachedGameState->OnMiniGameScoreBoardChanged.RemoveAll(this);
	
	Super::NativeDestruct();
}

void UDDMiniGamePlayerInfo::InitializePlayerInfo(APlayerState* InPlayerState)
{
	CachedPlayerState = Cast<ADDBasePlayerState>(InPlayerState);	
	if (!CachedPlayerState) return;
	
	if (PlayerNameText)
	{
		PlayerNameText->SetText(FText::FromName(CachedPlayerState->PlayerGameData.PlayerDisplayName));
		
		PlayerNameText->SetColorAndOpacity(CachedPlayerState->PlayerGameData.PlayerColor);
	}
	
	CachedGameState = GetWorld()->GetGameState<ADDMiniGameStateBase>();
	if (CachedGameState)
	{
		CachedGameState->OnMiniGameScoreBoardChanged.AddDynamic(this, &ThisClass::OnScoreBoardChanged);
		OnScoreBoardChanged(); 
	}
}

void UDDMiniGamePlayerInfo::UpdateScore(int32 InScore)
{
	if (ScoreText)
	{
		FString ScoreString = InScore >= 0 ? 
			FString::Printf(TEXT("+%d"), InScore) :
			FString::Printf(TEXT("%d"), InScore);
		
		ScoreText->SetText(FText::FromString(ScoreString));
	}
}

void UDDMiniGamePlayerInfo::OnScoreBoardChanged()
{
	if (!CachedGameState || !CachedPlayerState) return;

	for (const FMiniGameScoreEntry& Entry : CachedGameState->GetScoreBoard())
	{
		if (Entry.PlayerId == CachedPlayerState->GetPlayerId())
		{
			UpdateScore(Entry.Score);
			return;
		}
	}
}
