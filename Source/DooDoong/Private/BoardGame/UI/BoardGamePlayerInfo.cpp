#include "BoardGame/UI/BoardGamePlayerInfo.h"
#include "AbilitySystem/Attributes/DDHealthSet.h"
#include "AbilitySystem/Attributes/DDPointSet.h"
#include "Base/Player/DDBasePlayerState.h"
#include "Components/TextBlock.h"

void UBoardGamePlayerInfo::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBoardGamePlayerInfo::InitializePlayerInfo(APlayerState* PlayerState)
{
	CurrentPlayerState = Cast<ADDBasePlayerState>(PlayerState);
	if (!CurrentPlayerState) return;
	
	if(PlayerNameText)
	{
		PlayerNameText->SetText(FText::FromName(CurrentPlayerState->GetPlayerDisplayName()));
		
		PlayerColor = CurrentPlayerState->PlayerGameData.PlayerColor; 
		PlayerNameText->SetColorAndOpacity(PlayerColor);
	}
	
	if (UDDHealthSet* HealthSet = CurrentPlayerState->GetHealthSet())
	{
		HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::UpdateHealth);
		
		UpdateHealth(HealthSet->GetHealth(), HealthSet->GetMaxHealth());
	}
	
	if (UDDPointSet* PointSet = CurrentPlayerState->GetPointSet())
	{
		PointSet->OnPointChanged.AddUObject(this, &ThisClass::UpdateCoinCount);
		UpdateCoinCount(PointSet->GetCoin());
		
		PointSet->OnTrophyChanged.AddUObject(this, &ThisClass::UpdateTrophyCount);
		UpdateTrophyCount(PointSet->GetTrophy());
	}
	
	CurrentPlayerState->OnRankChanged.AddUObject(this, &ThisClass::UpdateCurrentRank);
    UpdateCurrentRank(CurrentPlayerState->PlayerGameData.CurrentRank);
}

void UBoardGamePlayerInfo::NativeDestruct()
{
	if (CurrentPlayerState)
	{
		if (UDDHealthSet* HealthSet = CurrentPlayerState->GetHealthSet())
		{
			HealthSet->OnHealthChanged.RemoveAll(this);
		}
		
		if (UDDPointSet* PointSet = CurrentPlayerState->GetPointSet())
		{
			PointSet->OnPointChanged.RemoveAll(this);
			PointSet->OnTrophyChanged.RemoveAll(this);
		}
		
		CurrentPlayerState->OnRankChanged.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UBoardGamePlayerInfo::UpdateHealth(int32 Cur, int32 Max)
{
	if (HealthText)
	{
		FString HPString = FString::Printf(TEXT("체력 : %d / %d"), Cur, Max);
		HealthText->SetText(FText::FromString(HPString));
	}
}

void UBoardGamePlayerInfo::UpdateCoinCount(int32 Coin)
{
	if (CoinText)
	{
		FString CoinString = FString::Printf(TEXT("코인 : %d"), Coin); 
		CoinText->SetText(FText::FromString(CoinString));
	}
}

void UBoardGamePlayerInfo::UpdateTrophyCount(int32 Trophy)
{
	if (TrophyText)
	{
		FString TrophyString = FString::Printf(TEXT("트로피 : %d"), Trophy);
		TrophyText->SetText(FText::FromString(TrophyString));
	}
}

void UBoardGamePlayerInfo::UpdateCurrentRank(int32 CurrentRank)
{
	if (IsValid(CurrentRankText))
    {
        // 1st, 2nd, 3rd, 4th 같은 접미사를 원하신다면 조건문으로 처리할 수 있습니다.
        FString Suffix = TEXT("th");
        if (CurrentRank == 1) Suffix = TEXT("st");
        else if (CurrentRank == 2) Suffix = TEXT("nd");
        else if (CurrentRank == 3) Suffix = TEXT("rd");

        FString RankString = FString::Printf(TEXT("%d%s"), CurrentRank, *Suffix);
        CurrentRankText->SetText(FText::FromString(RankString));
    }
}
