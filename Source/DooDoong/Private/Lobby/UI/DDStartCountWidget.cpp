#include "Lobby/UI/DDStartCountWidget.h"
#include "Components/TextBlock.h"
#include "Lobby/Game/DDLobbyGameState.h"

void UDDStartCountWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 바인드 
	CurrentGameState = GetWorld()->GetGameState<ADDLobbyGameState>();
	if (CurrentGameState)
	{
		CurrentGameState->OnCountDownChanged.AddDynamic(this, &ThisClass::UpdateCount);
	}
	
}

void UDDStartCountWidget::NativeDestruct()
{
	// 바인드 해제 
	CurrentGameState->OnCountDownChanged.RemoveAll(this);
	
	Super::NativeDestruct();
}

void UDDStartCountWidget::UpdateCount(int32 inCount)
{
	if (CountText)
	{
		FString CountString = FString::Printf(TEXT("%d"), inCount);
		CountText->SetText(FText::FromString(CountString));
	}
}
