#include "UI/Widgets/DDLobbyEnteranceWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Lobby/Player/DDLobbyPlayerController.h"

void UDDLobbyEnteranceWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayButton->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
	
	NicknameEditableText->OnTextChanged.AddDynamic(this, &ThisClass::OnNicknameTextChanged);
	NicknameEditableText->OnTextCommitted.AddDynamic(this, &ThisClass::OnNicknameTextCommitted);
	
	PlayButton->SetIsEnabled(false);
}

void UDDLobbyEnteranceWidget::OnNicknameTextChanged(const FText& Text)
{
	PlayButton->SetIsEnabled(!Text.IsEmpty());
}

void UDDLobbyEnteranceWidget::OnNicknameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        if (PlayButton->GetIsEnabled())
        {
            OnPlayButtonClicked();
        }
    }
}

void UDDLobbyEnteranceWidget::OnNicknameSubmitResult(bool bSuccess, const FString& ErrorMessage)
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

void UDDLobbyEnteranceWidget::OnPlayButtonClicked()
{
	ADDLobbyPlayerController* LobbyPlayerController = GetOwningPlayer<ADDLobbyPlayerController>();
	if (IsValid(LobbyPlayerController))
	{
		FText Nickname = NicknameEditableText->GetText();
		
		LobbyPlayerController->Server_SubmitNickname(FName(*Nickname.ToString()));
		
		PlayButton->SetIsEnabled(false);
	}
}

void UDDLobbyEnteranceWidget::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(
		this,
		GetOwningPlayer(),
		EQuitPreference::Quit,
		false);
}
