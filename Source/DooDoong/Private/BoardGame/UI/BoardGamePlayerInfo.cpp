#include "BoardGame/UI/BoardGamePlayerInfo.h"
#include "AbilitySystem/Attributes/DDHealthSet.h"
#include "AbilitySystem/Attributes/DDPointSet.h"
#include "Base/Player/DDBasePlayerState.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UBoardGamePlayerInfo::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBoardGamePlayerInfo::InitializePlayerInfo(APlayerState* PlayerState)
{
	ClearPlayerInfo(); 
	
	CurrentPlayerState = Cast<ADDBasePlayerState>(PlayerState);
	if (!CurrentPlayerState) return;
	
	if(PlayerNameText)
	{
		PlayerNameText->SetText(FText::FromName(CurrentPlayerState->GetPlayerDisplayName()));
		
		PlayerColor = CurrentPlayerState->PlayerGameData.PlayerColor; 
	}
	
	if (UDDHealthSet* HealthSet = CurrentPlayerState->GetHealthSet())
	{
		HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::UpdateHealth);
		
		UpdateHealth(HealthSet->GetHealth(), HealthSet->GetMaxHealth());
	}
	
	if (UDDPointSet* PointSet = CurrentPlayerState->GetPointSet())
	{
		PointSet->OnCoinChanged.AddUObject(this, &ThisClass::UpdateCoinCount);
		UpdateCoinCount(PointSet->GetCoin());
		
		PointSet->OnTrophyChanged.AddUObject(this, &ThisClass::UpdateTrophyCount);
		UpdateTrophyCount(PointSet->GetTrophy());
	}
	
	if (BackgroundImage)
	{
		BackgroundDynMat = BackgroundImage->GetDynamicMaterial();
		if (BackgroundDynMat)
		{
			BackgroundDynMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(PlayerColor));
		}
	}
	
	CurrentPlayerState->OnRankChanged.AddUObject(this, &ThisClass::UpdateCurrentRank);
    UpdateCurrentRank(CurrentPlayerState->PlayerGameData.CurrentRank);
}

void UBoardGamePlayerInfo::ClearPlayerInfo()
{
	if (CurrentPlayerState)
	{
		if (UDDHealthSet* HealthSet = CurrentPlayerState->GetHealthSet())
		{
			HealthSet->OnHealthChanged.RemoveAll(this);
		}
		
		if (UDDPointSet* PointSet = CurrentPlayerState->GetPointSet())
		{
			PointSet->OnCoinChanged.RemoveAll(this);
			PointSet->OnTrophyChanged.RemoveAll(this);
		}
		
		CurrentPlayerState->OnRankChanged.RemoveAll(this);
	}
}

void UBoardGamePlayerInfo::NativeDestruct()
{
	ClearPlayerInfo();
	
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
		FString Suffix =
		  (CurrentRank == 1) ? TEXT("st") :  
		  (CurrentRank == 2) ? TEXT("nd") :
		  (CurrentRank == 3) ? TEXT("rd") : TEXT("th"); 
		
		FString RankString = FString::Printf(TEXT("%d%s"), CurrentRank, *Suffix);
		CurrentRankText->SetText(FText::FromString(RankString));
		
		if(RankColors.Num() <= 0) return;  
		
		const int32 ColorIndex = RankColors.IsValidIndex(CurrentRank) ? CurrentRank : 0;
		if (RankColors.IsValidIndex(ColorIndex))
		{
			CurrentRankText->SetColorAndOpacity(RankColors[ColorIndex]);
		}
	}
	
}
