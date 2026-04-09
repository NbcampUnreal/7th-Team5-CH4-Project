#include "Base/Player/DDLobbyPlayerController.h"
#include "Base/Game/DDLobbyGameMode.h"
#include "Base/Player/DDBasePlayerState.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/GameStateBase.h"
#include "UI/HUD/DDHUD.h"
#include "UI/Widgets/DDTitleLayoutWidget.h"

bool ADDLobbyPlayerController::Server_SubmitNickname_Validate(const FName& InNickname)
{
	return !InNickname.IsNone() && InNickname.ToString().Len() <= 12;
}

void ADDLobbyPlayerController::Server_SubmitNickname_Implementation(const FName& InNickname)
{
	AGameStateBase* GameState = GetWorld()->GetGameState();
    if (IsValid(GameState))
    {
        // 모든 플레이어의 닉네임 중복 검사
        for (APlayerState* PS : GameState->PlayerArray)
        {
            ADDBasePlayerState* DDPS = Cast<ADDBasePlayerState>(PS);
            if (DDPS && DDPS->PlayerGameData.PlayerDisplayName == InNickname)
            {
                Client_ReceiveNicknameFailure(TEXT("이미 존재하는 닉네임입니다."));
                return;
            }
        }
    }
	
	ADDLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ADDLobbyGameMode>();
	if (IsValid(LobbyGameMode))
	{
		LobbyGameMode->ProcessPlayerJoin(this, InNickname);
	}
}

void ADDLobbyPlayerController::Client_JoinLobby_Implementation()
{
	ADDHUD* CommonHUD = Cast<ADDHUD>(GetHUD());
    
    if (IsValid(CommonHUD))
    {
       // 로비 UI를 화면에서 제거합니다.
       CommonHUD->HideMainWidget();
    }

	// 닉네임 입력 완료 후 캐릭터 조작 권한 활성화
	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}

void ADDLobbyPlayerController::Client_ReceiveNicknameFailure_Implementation(const FString& ErrorMessage)
{
	// 로컬 뷰포트에서 UI 위젯을 찾아 버튼을 다시 활성화
    TArray<UUserWidget*> FoundWidgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UDDTitleLayoutWidget::StaticClass(), false);

    if (FoundWidgets.Num() > 0)
    {
        if (UDDTitleLayoutWidget* TitleWidget = Cast<UDDTitleLayoutWidget>(FoundWidgets[0]))
        {
            // 위젯 버튼 활성화 및 에러 메시지 처리 요청
            TitleWidget->OnNicknameSubmitResult(false, ErrorMessage);
        }
    }
}
