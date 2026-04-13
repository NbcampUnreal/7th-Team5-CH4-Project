#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseGameWidget.generated.h"

class UBasePlayerRowWidget;
class UVerticalBox;

UCLASS()
class DOODOONG_API UBaseGameWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitHUD(const TArray<class APlayerState*>& PlayerStates);

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UBasePlayerRowWidget> PlayerRowClass;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* PlayerListBox;

private:
    UPROPERTY()
    TArray<UBasePlayerRowWidget*> PlayerRows;
};