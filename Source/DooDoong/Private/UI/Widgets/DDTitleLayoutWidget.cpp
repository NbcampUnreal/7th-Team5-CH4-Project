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

	// 닉네임 입력 변화 감지
	NicknameEditableText->OnTextChanged.AddDynamic(this, &ThisClass::OnNicknameTextChanged);
	NicknameEditableText->OnTextCommitted.AddDynamic(this, &ThisClass::OnNicknameTextCommitted);

	// 처음에는 닉네임이 없으므로 버튼 비활성화
	PlayButton->SetIsEnabled(false);
}

void UDDTitleLayoutWidget::OnNicknameTextChanged(const FText& Text)
{
	// 텍스트가 비어있지 않은지 확인 후 버튼 활성화 여부 결정
	PlayButton->SetIsEnabled(!Text.IsEmpty());
}

void UDDTitleLayoutWidget::OnNicknameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	// 입력 완료 방식이 'Enter 키 누름' 인지 확인합니다.
    if (CommitMethod == ETextCommit::OnEnter)
    {
        if (PlayButton->GetIsEnabled())
        {
            OnPlayButtonClicked();
        }
    }
}

void UDDTitleLayoutWidget::OnPlayButtonClicked()
{
	ADDLobbyPlayerController* LobbyPlayerController = GetOwningPlayer<ADDLobbyPlayerController>();
	if (IsValid(LobbyPlayerController))
	{
		FText Nickname = NicknameEditableText->GetText();

		// 서버로 닉네임을 전송해 달라고 컨트롤러에 요청
		LobbyPlayerController->Server_SubmitNickname(Nickname.ToString());

		// 버튼을 연타하지 못하도록 비활성화
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


