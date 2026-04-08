#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"

// 반드시 마지막에 위치해야 함
#include "WBP_MiniGameReady.generated.h"

UCLASS()
class DOODOONG_API UWBP_MiniGameReady : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    UButton* ReadyButton;

    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnReadyButtonClicked();
};