#include "UI/Widgets/DDLobbyNicknameSubmitWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Lobby/Player/DDLobbyPlayerController.h"

void UDDLobbyNicknameSubmitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayButton->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
	
	NicknameEditableText->OnTextChanged.AddDynamic(this, &ThisClass::OnNicknameTextChanged);
	NicknameEditableText->OnTextCommitted.AddDynamic(this, &ThisClass::OnNicknameTextCommitted);
	
	PlayButton->SetIsEnabled(false);
}

void UDDLobbyNicknameSubmitWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UDDLobbyNicknameSubmitWidget::OnNicknameTextChanged(const FText& Text)
{
	PlayButton->SetIsEnabled(!Text.IsEmpty());
}

void UDDLobbyNicknameSubmitWidget::OnNicknameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        if (PlayButton->GetIsEnabled())
        {
            OnPlayButtonClicked();
        }
    }
}

void UDDLobbyNicknameSubmitWidget::OnNicknameSubmitResult(bool bSuccess, const FString& ErrorMessage)
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

void UDDLobbyNicknameSubmitWidget::OnPlayButtonClicked()
{
	ADDLobbyPlayerController* LobbyPlayerController = GetOwningPlayer<ADDLobbyPlayerController>();
	if (IsValid(LobbyPlayerController))
	{
		FText Nickname = NicknameEditableText->GetText();
		
		LobbyPlayerController->Server_SubmitNickname(FName(*Nickname.ToString()));
		
		PlayButton->SetIsEnabled(false);
	}
}

void UDDLobbyNicknameSubmitWidget::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(
		this,
		GetOwningPlayer(),
		EQuitPreference::Quit,
		false);
}
