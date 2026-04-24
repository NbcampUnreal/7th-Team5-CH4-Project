#include "Lobby/UI/DDLobbyWidget.h"

#include "Common/DDLogManager.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Lobby/Game/DDLobbyGameState.h"
#include "Lobby/Player/DDLobbyPlayerController.h"

void UDDLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 버튼 바인드 
	if (ReadyButton) ReadyButton->OnClicked.AddDynamic(this, &ThisClass::OnClickReadButton);

	if (QuitButton) QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnClickQuitButton);
	
	// 텍스트 바인드 
	CurrentGameState = GetWorld()->GetGameState<ADDLobbyGameState>();
	if (CurrentGameState)
	{
		CurrentGameState->OnPlayerCountChanged.AddDynamic(this, &ThisClass::UpdatePlayerCount);
		UpdatePlayerCount(CurrentGameState->PlayerCount);
		
		CurrentGameState->OnReadyCountChanged.AddDynamic(this, &ThisClass::UpdateReadyCount);
		UpdateReadyCount(CurrentGameState->ReadyCount);
	}
	else
	{
		LOG_KMS(Warning, TEXT(" [Lobby Widget] : Fail to Get GameState.")); 
	}
}

void UDDLobbyWidget::NativeDestruct()
{
	// 바인드 해제 
	if (CurrentGameState)
	{
		CurrentGameState->OnReadyCountChanged.RemoveAll(this);
		CurrentGameState->OnPlayerCountChanged.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UDDLobbyWidget::OnClickReadButton()
{
	bIsReady = !bIsReady;

	ReadyButton->SetStyle(bIsReady ? ReadyStyle : NormalStyle);
	ReadyText->SetText(FText::FromString(bIsReady ? TEXT("준비됨") : TEXT("준비")));

	// 서버에 준비 상태 전달 필요
	ADDLobbyPlayerController* PC = GetOwningPlayer<ADDLobbyPlayerController>();
	 if (PC) PC->Server_RequestReady(bIsReady); 
}

void UDDLobbyWidget::OnClickQuitButton()
{
	// 게임 종료 
	UKismetSystemLibrary::QuitGame(
		this,
		GetOwningPlayer(),
		EQuitPreference::Quit,
		false
	);
}

void UDDLobbyWidget::UpdatePlayerCount(int32 NewCount)
{
	PlayerCount = NewCount;

	FString ReadyString =
		FString::Printf(TEXT("%d / %d"), ReadyCount, PlayerCount);
	
	if (ReadyCountText) 
		ReadyCountText->SetText(FText::FromString(ReadyString));
	
	FString PlayerCountString =
		FString::Printf(TEXT("현재 인원 : %d"), PlayerCount);
	
	if (PlayerCountText)
		PlayerCountText->SetText(FText::FromString(PlayerCountString));
}

void UDDLobbyWidget::UpdateReadyCount(int32 NewCount)
{
	ReadyCount = NewCount;
	
	FString ReadyString =
		FString::Printf(TEXT("%d / %d"), ReadyCount, PlayerCount);
	
	if (ReadyCountText)
		ReadyCountText->SetText(FText::FromString(ReadyString));
}

