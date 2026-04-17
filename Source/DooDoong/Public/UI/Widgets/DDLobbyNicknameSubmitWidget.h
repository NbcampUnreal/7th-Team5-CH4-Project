#pragma once

#include "Blueprint/UserWidget.h"
#include "DDLobbyNicknameSubmitWidget.generated.h"

class UButton;
class UEditableText;

UCLASS()
class DOODOONG_API UDDLobbyNicknameSubmitWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();

	UFUNCTION()
	void OnNicknameTextChanged(const FText& Text);
	
	UFUNCTION()
    void OnNicknameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
public:
	UFUNCTION()
	void OnNicknameSubmitResult(bool bSuccess, const FString& ErrorMessage);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> PlayButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> ExitButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UEditableText> NicknameEditableText;
};




