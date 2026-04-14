#include "UI/Widgets/DDLobbyWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Lobby/Player/DDLobbyPlayerController.h"

void UDDLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayButton->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
	
	NicknameEditableText->OnTextChanged.AddDynamic(this, &ThisClass::OnNicknameTextChanged);
	NicknameEditableText->OnTextCommitted.AddDynamic(this, &ThisClass::OnNicknameTextCommitted);
	
	PlayButton->SetIsEnabled(false);
}

void UDDLobbyWidget::OnNicknameTextChanged(const FText& Text)
{
	PlayButton->SetIsEnabled(!Text.IsEmpty());
}

void UDDLobbyWidget::OnNicknameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        if (PlayButton->GetIsEnabled())
        {
            OnPlayButtonClicked();
        }
    }
}

void UDDLobbyWidget::OnNicknameSubmitResult(bool bSuccess, const FString& ErrorMessage)
{
	if (bSuccess)
    {
        RemoveFromParent(); 
    }
    else
    {
        PlayButton->SetIsEnabled(true);
    	
        NicknameEditableText->SetKeyboardFocus();
    }
}

void UDDLobbyWidget::OnPlayButtonClicked()
{
	ADDLobbyPlayerController* LobbyPlayerController = GetOwningPlayer<ADDLobbyPlayerController>();
	if (IsValid(LobbyPlayerController))
	{
		FText Nickname = NicknameEditableText->GetText();
		
		LobbyPlayerController->Server_SubmitNickname(FName(*Nickname.ToString()));
		
		PlayButton->SetIsEnabled(false);
	}
}

void UDDLobbyWidget::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(
		this,
		GetOwningPlayer(),
		EQuitPreference::Quit,
		false);
}
