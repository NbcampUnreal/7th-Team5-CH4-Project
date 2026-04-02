#include "Base/Player/DDLobbyPlayerController.h"
#include "Base/Game/DDLobbyGameMode.h"
#include "UI/HUD/DDLobbyHUD.h"

bool ADDLobbyPlayerController::Server_SubmitNickname_Validate(const FString& InNickname)
{
	return !InNickname.IsEmpty();
}

void ADDLobbyPlayerController::Server_SubmitNickname_Implementation(const FString& InNickname)
{
	ADDLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ADDLobbyGameMode>();
	if (IsValid(LobbyGameMode))
	{
		LobbyGameMode->ProcessPlayerJoin(this, InNickname);
	}
}

void ADDLobbyPlayerController::Client_JoinLobby_Implementation()
{
	ADDLobbyHUD* LobbyHUD = Cast<ADDLobbyHUD>(GetHUD());
	if (IsValid(LobbyHUD))
	{
		LobbyHUD->HideLobbyUI();
	}

	// 닉네임 입력 완료 후 캐릭터 조작 권한 활성화
	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}
