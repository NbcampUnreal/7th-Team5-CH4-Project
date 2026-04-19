#include "UI/Widgets/MiniGameReadyWidget.h"
#include "System/MiniGame/DDMiniGameManager.h"
#include "Base/MiniGame/DDMiniGameStateBase.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/MultiLineEditableTextBox.h"
#include "System/MiniGame/DDMiniGameDefinition.h"
#include "Components/TextBlock.h"

void UMiniGameReadyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	CurrentGameState = GetWorld()->GetGameState<ADDMiniGameStateBase>();
	if (!CurrentGameState) return;
	
	CurrentGameState->OnMiniGameReadyStateChanged.AddDynamic(this, &UMiniGameReadyWidget::OnReadyStateChanged);
	OnReadyStateChanged(CurrentGameState->GetReadyPlayerCount(), CurrentGameState->GetTotalParticipantCount());
	
	UDDMiniGameManager* MiniGameManager = GetGameInstance()->GetSubsystem<UDDMiniGameManager>();
	if (IsValid(MiniGameManager))
	{
		UDDMiniGameDefinition* Definition = MiniGameManager->GetActiveDefinition();
		
		if (MiniGameTitle)
		{
			MiniGameTitle->SetText(FText::FromName(Definition->MiniGameID));
		}
		
		if (DescriptionText)
		{
			DescriptionText->SetText(Definition->Description);
		}
		
		if (ThumbnailImage)
		{
			ThumbnailImage->SetBrushFromTexture(Definition->MiniGameThumbnail);
		}
	}
}

void UMiniGameReadyWidget::NativeDestruct()
{
	if (CurrentGameState)
	{
		CurrentGameState->OnMiniGameReadyStateChanged.RemoveDynamic(this, &ThisClass::OnReadyStateChanged);
	}
	
	Super::NativeDestruct();
}

void UMiniGameReadyWidget::OnReadyButtonClicked()
{
	ADDBasePlayerController* PC = Cast<ADDBasePlayerController>(GetOwningPlayer());
	if (!PC) return;
	
	PC->Server_RequestPlayerReady();

	if (ReadyStateText)
	{
		ReadyButton->SetIsEnabled(false);
	}
	
	if (ReadyButtonText)
	{
		FString ReadyString = FString(TEXT("준비 완료"));
		ReadyButtonText->SetText(FText::FromString(ReadyString));
	}
}

void UMiniGameReadyWidget::OnReadyStateChanged(int32 ReadyCount, int32 TotalCount)
{
	FString NewReadyString = FString::Printf(TEXT("준비 인원 %d / %d"), ReadyCount, TotalCount);
	
	if (ReadyStateText)
	{
		ReadyStateText->SetText(FText::FromString(NewReadyString));
	}
}
