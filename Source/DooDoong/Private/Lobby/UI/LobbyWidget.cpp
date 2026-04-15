#include "Lobby/UI/LobbyWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Lobby/Player/DDLobbyPlayerController.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ReadyButton) ReadyButton->OnClicked.AddDynamic(this, &ThisClass::OnClickReadButton);

	if (QuitButton) QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnClickQuitButton);
}

void ULobbyWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void ULobbyWidget::OnClickReadButton()
{
	bIsReady = !bIsReady;

	ReadyButton->SetStyle(bIsReady ? ReadyStyle : NormalStyle);
	ReadyText->SetText(FText::FromString(bIsReady ? "준비됨" : "준비"));

	// 서버에 준비 상태 전달 필요
	ADDLobbyPlayerController* PC = GetOwningPlayer<ADDLobbyPlayerController>();
	 if (PC) PC->Server_RequestReady(bIsReady); 
}

void ULobbyWidget::OnClickQuitButton()
{
}

void ULobbyWidget::UpdatePlayerCount()
{
}

void ULobbyWidget::UpdateReadyCount()
{
	
}
