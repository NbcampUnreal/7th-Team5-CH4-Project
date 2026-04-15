#include "Lobby/Player/DDLobbyPlayerController.h"
#include "Lobby/Game/DDLobbyGameMode.h"
#include "UI/HUD/DDHUD.h"
#include "UI/Widgets/DDLobbyWidget.h"

void ADDLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// 입력모드 UI Only  
	ToggleUIInputMode(true); 
}

bool ADDLobbyPlayerController::Server_SubmitNickname_Validate(const FName& InNickname)
{
	return true;
}

void ADDLobbyPlayerController::Server_SubmitNickname_Implementation(const FName& InNickname)
{
	ADDLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ADDLobbyGameMode>();
	if (!IsValid(LobbyGameMode)) return; 
	
	FString ErrorMessage;
	if (!LobbyGameMode->TryRegisterPlayerNickname(this, InNickname, ErrorMessage))
	{
		// 게임 모드에서 등록 실패시 에러 메시지 전달 
		Client_ReceiveNicknameFailure(ErrorMessage);
	}
}

void ADDLobbyPlayerController::Client_JoinLobby_Implementation()
{
	ADDHUD* LobbyHUD = Cast<ADDHUD>(GetHUD());
    
    if (IsValid(LobbyHUD))
    {
       // 로비 UI를 화면에서 제거합니다.
       LobbyHUD->ToggleWidgetVisibility(false);
    }

	// 닉네임 입력 완료 후 캐릭터 조작 권한 활성화
	ToggleUIInputMode(false);
}

void ADDLobbyPlayerController::Client_ReceiveNicknameFailure_Implementation(const FString& ErrorMessage)
{
	// 1. HUD 가져오기
	ADDHUD* LobbyHUD = Cast<ADDHUD>(GetHUD());
	if (!IsValid(LobbyHUD)) return;
	
	// 2. HUD가 관리중인 메인 위젯을 가져와서 실행 
	if (UDDLobbyWidget* LobbyWidget = LobbyHUD->GetCurrentWidget<UDDLobbyWidget>())
	{
		LobbyWidget->OnNicknameSubmitResult(false, ErrorMessage);
	}
}

void ADDLobbyPlayerController::ToggleUIInputMode(bool bUIOnly)
{
	if (bUIOnly)
	{
		FInputModeUIOnly Mode;
		SetInputMode(Mode);
		bShowMouseCursor = true;
	}
	else
	{
		FInputModeGameOnly Mode;
		SetInputMode(Mode);
		bShowMouseCursor = false;
	}
}
