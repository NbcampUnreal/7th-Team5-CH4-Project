#include "Lobby/UI/DDStartCountWidget.h"
#include "Components/TextBlock.h"



void UDDStartCountWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 바인드 
}

void UDDStartCountWidget::NativeDestruct()
{
	// 바인드 해제 
	
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
