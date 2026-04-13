#include "UI/Widgets/BaseGameWidget.h"
#include "UI/Widgets/BasePlayerRowWidget.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameStateBase.h"
#include "Components/VerticalBoxSlot.h"

void UBaseGameWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Warning, TEXT("GameWidget Constructed"));
    BindToGameState();
}

void UBaseGameWidget::BindToGameState()
{
    UWorld* World = GetWorld();
    if (!World) return;

    AGameStateBase* GS = World->GetGameState();
    if (!GS)
    {
        World->GetTimerManager().SetTimerForNextTick(
            this,
            &UBaseGameWidget::BindToGameState
        );
        return;
    }

    // PlayerArray는 기본적으로 RepNotify 없음 → polling or custom delegate 필요
    // 최소 구현: 일단 한 번 갱신
    RefreshPlayerList();
}


void UBaseGameWidget::HandlePlayerStateChanged()
{
    RefreshPlayerList();
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




void UBaseGameWidget::RefreshPlayerList()
{
    UWorld* World = GetWorld();
    if (!World) return;

    AGameStateBase* GS = World->GetGameState();
    if (!GS) return;

    const TArray<APlayerState*>& PlayerStates = GS->PlayerArray;

    UE_LOG(LogTemp, Warning, TEXT("RefreshPlayerList Called: %d"), PlayerStates.Num());

    if (!PlayerRowClass || !PlayerListBox)
        return;

    PlayerListBox->ClearChildren();
    PlayerRows.Empty();

    for (APlayerState* PS : PlayerStates)
    {
        if (!PS) continue;

        UBasePlayerRowWidget* Row =
            CreateWidget<UBasePlayerRowWidget>(this, PlayerRowClass);

        if (!Row) continue;

        Row->Init(PS);

        PlayerListBox->AddChild(Row);
        PlayerRows.Add(Row);
    }
}