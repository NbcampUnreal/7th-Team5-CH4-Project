#include "Lobby/UI/DDLobbyWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Lobby/Player/DDLobbyPlayerController.h"

void UDDLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ReadyButton) ReadyButton->OnClicked.AddDynamic(this, &ThisClass::OnClickReadButton);

	if (QuitButton) QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnClickQuitButton);
}

void UDDLobbyWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UDDLobbyWidget::OnClickReadButton()
{
	bIsReady = !bIsReady;

	ReadyButton->SetStyle(bIsReady ? ReadyStyle : NormalStyle);
	ReadyText->SetText(FText::FromString(bIsReady ? "준비됨" : "준비"));

	// 서버에 준비 상태 전달 필요
	ADDLobbyPlayerController* PC = GetOwningPlayer<ADDLobbyPlayerController>();
	 if (PC) PC->Server_RequestReady(bIsReady); 
}

void UDDLobbyWidget::OnClickQuitButton()
{
}

void UDDLobbyWidget::UpdatePlayerCount()
{
}

void UDDLobbyWidget::UpdateReadyCount()
{
	
}
