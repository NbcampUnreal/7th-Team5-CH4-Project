#include "Base/Player/DDLobbyPlayerController.h"
#include "Base/Game/DDLobbyGameMode.h"
#include "UI/HUD/DDLobbyHUD.h"

bool ADDLobbyPlayerController::Server_SubmitNickname_Validate(const FString& InNickname)
{
	// 닉네임 입력 방어 로직
	return !InNickname.IsEmpty() && InNickname.Len() <= 12;
}

void ADDLobbyPlayerController::Server_SubmitNickname_Implementation(const FString& InNickname)
{
	// 클라이언트의 요청을 받아 서버 게임모드로 전달합니다.
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
