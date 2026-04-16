#include "BoardGame/UI/BoardGameWidget.h"
#include "BoardGame/Game/DDBoardGameState.h"
#include "Components/TextBlock.h"

void UBoardGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ADDBoardGameState* GS = GetWorld()->GetGameState<ADDBoardGameState>();
	if (!GS) return; 
	
	GS->OnStateTimerChanged.AddDynamic(this, &ThisClass::UpdateTimeText);
	GS->OnRoundChanged.AddDynamic(this, &ThisClass::UpdateRemainTurn);
}

void UBoardGameWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UBoardGameWidget::UpdateTimeText(int32 InTime)
{
	if (RemainTimeText)
	{
		FString TimeString = FString::Printf(TEXT("남은 시간 : %d"), InTime); 
		RemainTimeText->SetText(FText::FromString(TimeString));
	}
}

void UBoardGameWidget::UpdateRemainTurn(int32 InRemainTurn)
{
	if (RemainTurnText)
	{
		FString TurnString = FString::Printf(TEXT("남은 턴 : %d"), InRemainTurn);
		RemainTurnText->SetText(FText::FromString(TurnString));
	}
}
