#include "Lobby/UI/DDErrorMessageWidget.h"
#include "Components/TextBlock.h"

void UDDErrorMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDDErrorMessageWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UDDErrorMessageWidget::UpdateErrorMessage(const FString& InErrorMessage)
{
	if (ErrorMessageText)
	{
		ErrorMessageText->SetText(FText::FromString(InErrorMessage));
	}
}
