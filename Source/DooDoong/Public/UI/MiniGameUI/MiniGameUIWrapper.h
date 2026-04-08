#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniGameUIWrapper.generated.h"

UCLASS()
class DOODOONG_API UMiniGameUIWrapper : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 위젯 클래스 지정 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> ReadyWidgetClass;

    void UpdateReadyCount(int32 ReadyCount, int32 TotalCount);

private:
    /** 실제 생성된 위젯 */
    UPROPERTY()
    UUserWidget* ReadyWidget = nullptr;

public:
    void OpenReadyUI(APlayerController* PC);
    void CloseReadyUI(APlayerController* PC);
};