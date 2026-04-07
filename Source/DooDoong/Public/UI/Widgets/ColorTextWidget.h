#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "ColorTextWidget.generated.h"

class UTexture2D;

UCLASS()
class DOODOONG_API UColorTextWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    // 문자 → 텍스처
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font")
    TMap<FString, UTexture2D*> CharToTexture;

    // UMG에서 바인딩
    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* LetterContainer;

    UFUNCTION(BlueprintCallable)
    void SetText(const FString& InputText);
};

