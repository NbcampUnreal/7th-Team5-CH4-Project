#include "UI/Widgets/ColorTextWidget.h"
#include "Components/Image.h"
#include "Components/HorizontalBoxSlot.h"

void UColorTextWidget::SetText(const FString& InputText)
{
    if (!LetterContainer) return;

    LetterContainer->ClearChildren();

    for (int32 i = 0; i < InputText.Len(); i++)
    {
        FString Char = InputText.Mid(i, 1);

        if (CharToTexture.Contains(Char))
        {
            UTexture2D* Texture = CharToTexture[Char];
            if (!Texture) continue;

            UImage* Image = NewObject<UImage>(this);

            FSlateBrush Brush;
            Brush.SetResourceObject(Texture);
            Brush.ImageSize = FVector2D(64, 64); // 안전 고정값

            Image->SetBrush(Brush);

            LetterContainer->AddChildToHorizontalBox(Image);
        }
    }
}

