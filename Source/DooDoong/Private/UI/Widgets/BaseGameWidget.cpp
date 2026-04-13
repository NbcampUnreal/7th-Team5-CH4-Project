#include "UI/Widgets/BaseGameWidget.h"
#include "UI/Widgets/BasePlayerRowWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UBaseGameWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Warning, TEXT("GameWidget Constructed"));
}

void UBaseGameWidget::InitHUD(const TArray<APlayerState*>& PlayerStates)
{
    UE_LOG(LogTemp, Warning, TEXT("InitHUD Called"));

    if (!PlayerRowClass)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerRowClass is NULL"));
        return;
    }

    if (!PlayerListBox)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerListBox is NULL"));
        return;
    }

    PlayerListBox->ClearChildren();
    PlayerRows.Empty();

    for (APlayerState* PS : PlayerStates)
    {
        if (!PS) continue;

        UBasePlayerRowWidget* Row = CreateWidget<UBasePlayerRowWidget>(this, PlayerRowClass);

        if (!Row)
        {
            UE_LOG(LogTemp, Error, TEXT("Row Create Failed"));
            continue;
        }

        Row->Init(PS);

        PlayerListBox->AddChild(Row);
        PlayerRows.Add(Row);

        UE_LOG(LogTemp, Warning, TEXT("Row Added"));
    }
}