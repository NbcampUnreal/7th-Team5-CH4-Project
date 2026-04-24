#include "Lobby/Player/DDLobbyPlayerController.h"
#include "Common/DDLogManager.h"
#include "Lobby/Game/DDLobbyGameMode.h"
#include "System/DDGameplayTags.h"
#include "System/DDUIManagerSubsystem.h"

void ADDLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// 입력모드 UI Only  
	FInputModeGameAndUI Mode;
    SetInputMode(Mode);
    bShowMouseCursor = true;
}

void ADDLobbyPlayerController::Client_JoinLobby_Implementation()
{
	LOG_KMS(Warning, TEXT("플레이어 입장 성공")); 
	
	// 닉네임 입력 팝업 제거 
	Client_ClosePopUp(DDGameplayTags::Lobby_UI_NicknamePopup); 
	
	// Lobby 메인 열기 
	Client_SwitchGameLayer(DDGameplayTags::Lobby_UI_Main);
	
	// 닉네임 입력 완료 후 캐릭터 조작 권한 활성화
	FInputModeGameAndUI Mode;
	SetInputMode(Mode);
	
	ADDLobbyGameState* LobbyGameState = GetWorld()->GetGameState<ADDLobbyGameState>();
	if (IsValid(LobbyGameState))
	{
		LobbyGameState->OnCountDownChanged.AddDynamic(this, &ADDLobbyPlayerController::OnCountdownChanged);
	}
}

bool ADDLobbyPlayerController::Server_SubmitNickname_Validate(const FName& InNickname)
{
	return true;
}

void ADDLobbyPlayerController::Server_SubmitNickname_Implementation(const FName& InNickname)
{
	LOG_KMS(Warning, TEXT("닉네임 제출 : %s"), *InNickname.ToString());
	ADDLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ADDLobbyGameMode>();
	if (!IsValid(LobbyGameMode)) return; 
	
	FString ErrorMessage;
	if (!LobbyGameMode->TryRegisterPlayerNickname(this, InNickname, ErrorMessage))
	{
		// 게임 모드에서 등록 실패시 에러 메시지 전달 
		LOG_KMS(Warning, TEXT("%s"), *ErrorMessage);
		Client_ReceiveNicknameFailure(ErrorMessage);
		return; 
	}
}

void ADDLobbyPlayerController::Client_ReceiveNicknameFailure_Implementation(const FString& ErrorMessage)
{
	// Error Popup 그리기 
	Client_DrawErrorMessage(ErrorMessage, 3.f);
}

bool ADDLobbyPlayerController::Server_RequestReady_Validate(bool bIsReady)
{
	return true; 
}

void ADDLobbyPlayerController::Server_RequestReady_Implementation(bool bIsReady)
{
	// 서버에다가 Ready 요청 
	ADDLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ADDLobbyGameMode>();
	if (!IsValid(LobbyGameMode)) return; 
	
	LobbyGameMode->RequestPlayerReady(this, bIsReady);
}

void ADDLobbyPlayerController::OnCountdownChanged(int32 NewCountdown)
{
	UDDUIManagerSubsystem* UIManager = GetLocalPlayer()->GetSubsystem<UDDUIManagerSubsystem>();
	if (!UIManager) return;
	
	if (NewCountdown > 0 )
	{
		if (!UIManager->IsPopupOpen(DDGameplayTags::Lobby_UI_CountDown))
			UIManager->DrawPopup(DDGameplayTags::Lobby_UI_CountDown);
	}
	else
		UIManager->HidePopup(DDGameplayTags::Lobby_UI_CountDown);
}

