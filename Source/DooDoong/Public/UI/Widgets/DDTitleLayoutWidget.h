#pragma once

#include "Blueprint/UserWidget.h"
#include "DDTitleLayoutWidget.generated.h"

class UButton;
class UEditableText;

UCLASS()
class UDDTitleLayoutWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UDDTitleLayoutWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();

	UFUNCTION()
	void OnNicknameTextChanged(const FText& Text);
	
	UFUNCTION()
    void OnNicknameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> PlayButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> ExitButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UEditableText> NicknameEditableText;
};




