#include "UI/Widgets/DDTitleLayoutWidget.h"
#include "Base/Player/DDLobbyPlayerController.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"

UDDTitleLayoutWidget::UDDTitleLayoutWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDDTitleLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayButton->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
	
	NicknameEditableText->OnTextChanged.AddDynamic(this, &ThisClass::OnNicknameTextChanged);
	NicknameEditableText->OnTextCommitted.AddDynamic(this, &ThisClass::OnNicknameTextCommitted);
	
	PlayButton->SetIsEnabled(false);
}

void UDDTitleLayoutWidget::OnNicknameTextChanged(const FText& Text)
{
	PlayButton->SetIsEnabled(!Text.IsEmpty());
}

void UDDTitleLayoutWidget::OnNicknameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        if (PlayButton->GetIsEnabled())
        {
            OnPlayButtonClicked();
        }
    }
}

void UDDTitleLayoutWidget::OnNicknameSubmitResult(bool bSuccess, const FString& ErrorMessage)
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

void UDDTitleLayoutWidget::OnPlayButtonClicked()
{
	ADDLobbyPlayerController* LobbyPlayerController = GetOwningPlayer<ADDLobbyPlayerController>();
	if (IsValid(LobbyPlayerController))
	{
		FText Nickname = NicknameEditableText->GetText();
		
		LobbyPlayerController->Server_SubmitNickname(FName(*Nickname.ToString()));
		
		PlayButton->SetIsEnabled(false);
	}
}

void UDDTitleLayoutWidget::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(
		this,
		GetOwningPlayer(),
		EQuitPreference::Quit,
		false);
}




