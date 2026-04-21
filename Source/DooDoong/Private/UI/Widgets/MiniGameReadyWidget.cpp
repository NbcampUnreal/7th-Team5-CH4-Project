#include "UI/Widgets/MiniGameReadyWidget.h"
#include "Base/MiniGame/DDMiniGameStateBase.h"
#include "Base/MiniGame/DDMiniGameTypes.h"
#include "Base/Player/DDBasePlayerController.h"
#include "Common/DDLogManager.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/TextBlock.h"

void UMiniGameReadyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ReadyButton)
	{
		ReadyButton->OnClicked.AddDynamic(this, &ThisClass::OnReadyButtonClicked);
	}
}

void UMiniGameReadyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	TryBindGameInfo();
	TryBindGameState(); 
}

void UMiniGameReadyWidget::NativeDestruct()
{
	// 바인딩 해제 
	if (CurrentGameState)
	{
		CurrentGameState->OnMiniGameReadyStateChanged.RemoveDynamic(this, &ThisClass::OnReadyStateChanged);
	}
	
	// 아직 타이머 도는 중이라면 타이머 해제 
	GetWorld()->GetTimerManager().ClearTimer(GameStateRetryHandle);
	GetWorld()->GetTimerManager().ClearTimer(GameInfoRetryHandle);
	
	Super::NativeDestruct();
}

void UMiniGameReadyWidget::TryBindGameState()
{
	CurrentGameState = GetWorld()->GetGameState<ADDMiniGameStateBase>();
	if (!CurrentGameState)
	{
		LOG_KMS(Warning, TEXT("미니 게임 스테이트가 없어 다시 실행합니다."));
		
		GetWorld()->GetTimerManager().SetTimer(
			GameStateRetryHandle, 
			this, 
			&ThisClass::TryBindGameState,
			0.1f, 
			false
		);
		
		return;
	}
	
	// 준비 인원 수 갱신 델리게이트
	CurrentGameState->OnMiniGameReadyStateChanged.AddDynamic(
		this, &UMiniGameReadyWidget::OnReadyStateChanged);
	
	OnReadyStateChanged(CurrentGameState->GetReadyPlayerCount(), CurrentGameState->GetTotalParticipantCount());
}

void UMiniGameReadyWidget::TryBindGameInfo()
{
	ADDMiniGameStateBase* MiniGameState = GetWorld()->GetGameState<ADDMiniGameStateBase>();
	if (!MiniGameState)
	{
		LOG_KMS(Warning, TEXT("미니게임 스테이트가 없습니다"));
		GetWorld()->GetTimerManager().SetTimer(
			GameInfoRetryHandle, 
			this,
			&ThisClass::TryBindGameInfo,
			0.1f, 
			false
		);
		
		return;
	}

	const FMiniGameSetup& Setup = MiniGameState->GetMiniGameSetup();
	if (Setup.MiniGameID.IsNone())
	{
		return;
	}

	if (MiniGameTitle)
	{
		MiniGameTitle->SetText(Setup.DisplayName);
	}

	if (DescriptionText)
	{
		DescriptionText->SetText(Setup.Description);
	}

	if (ThumbnailImage && Setup.MiniGameThumbnail)
	{
		ThumbnailImage->SetBrushFromTexture(Setup.MiniGameThumbnail);
	}
}

void UMiniGameReadyWidget::OnReadyButtonClicked()
{
	ADDBasePlayerController* PC = Cast<ADDBasePlayerController>(GetOwningPlayer());
	if (!PC) return;
	
	PC->Server_RequestPlayerReady();

	if (ReadyButton)
	{
		ReadyButton->SetIsEnabled(false);
	}
	
	if (ReadyButtonText)
	{
		const FString ReadyString = FString(TEXT("준비 완료"));
		ReadyButtonText->SetText(FText::FromString(ReadyString));
	}
}

void UMiniGameReadyWidget::OnReadyStateChanged(int32 ReadyCount, int32 TotalCount)
{
	const FString NewReadyString = FString::Printf(TEXT("준비 인원 %d / %d"), ReadyCount, TotalCount);
	
	if (ReadyStateText)
	{
		ReadyStateText->SetText(FText::FromString(NewReadyString));
	}
}
