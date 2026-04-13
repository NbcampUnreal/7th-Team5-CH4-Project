#include "UI/Widgets/DDMiniGameReadyWidget.h"
#include "System/MiniGame/DDMiniGameManager.h"
#include "Components/TextBlock.h"

void UDDMiniGameReadyWidget::NativeConstruct()
{
    Super::NativeConstruct();


    BindToGameState();

    ApplyToUI();
    //ApplyToUI(); NativeConstruct 시점 = 아직 데이터 없음 -> 그래서 초기값(null / 0) 먼저 찍힘

    // 약간 늦게 한 번 더 (GameState replication 기다림)
    //GetWorld()->GetTimerManager().SetTimerForNextTick(this,
        //&UDDMiniGameReadyWidget::ApplyToUI);
    //이제 필요 없음 (Delegate가 대신함)
}

void UDDMiniGameReadyWidget::SetMiniGameData(const UDDMiniGameDefinition* Definition)
{
    if (!Definition) return;

    FallbackDefinition = Definition;

    ApplyToUI();
}

void UDDMiniGameReadyWidget::ApplyToUI()
{
    UE_LOG(LogTemp, Warning, TEXT("[MiniGameUI] ApplyToUI START"));

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("[MiniGameUI] World NULL"));
        return;
    }

    ADDMiniGameStateBase* GS =
        World->GetGameState<ADDMiniGameStateBase>();

    if (!GS)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MiniGameUI] GameState NOT READY"));
        return;
    }

    if (GS->MiniGameID.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("[MiniGameUI] MiniGameID NOT READY"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[MiniGameUI] MiniGameID: %s"),
        *GS->MiniGameID.ToString());

    FString Path = GetDefinitionPathByID(GS->MiniGameID);

    if (Path.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("[MiniGameUI] Path EMPTY"));
        return;
    }

    const UDDMiniGameDefinition* Def =
        LoadObject<UDDMiniGameDefinition>(nullptr, *Path);

    if (!Def)
    {
        Def = FallbackDefinition;
        UE_LOG(LogTemp, Error, TEXT("[MiniGameUI] Definition LOAD FAILED"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[MiniGameUI] Definition Loaded: %s"),
        *Def->DisplayName.ToString());

    // =====================
    // UI 적용
    // =====================

    if (GameNameText)
        GameNameText->SetText(Def->DisplayName);

    if (DescriptionText)
        DescriptionText->SetText(Def->Description);

    if (TimeLimitText)
        TimeLimitText->SetText(
            FText::AsNumber(Def->DefaultTimeLimitSeconds));

    if (GenreText)
        GenreText->SetText(
            FText::FromName(Def->Genre.GetTagName()));
}



FString UDDMiniGameReadyWidget::GetDefinitionPathByID(FName ID) const
{
    if (ID == "Shooter")
    {
        return TEXT("/Game/DooDoong/04_MiniGames/Shooter/Data/DA_Shooter.DA_Shooter");
    }
    else if (ID == "Platformer")
    {
        return TEXT("/Game/DooDoong/04_MiniGames/Platformer/Data/DA_Platformer.DA_Platformer");
    }

    UE_LOG(LogTemp, Error, TEXT("[MiniGameUI] Unknown MiniGameID: %s"),
        *ID.ToString());

    return TEXT("");
}




void UDDMiniGameReadyWidget::SetReadyCount(int32 Ready, int32 Total)
{
    if (ReadyText)
    {
        ReadyText->SetText(FText::FromString(
            FString::Printf(TEXT("%d / %d"), Ready, Total)
        ));
    }
}




void UDDMiniGameReadyWidget::BindToGameState()
{
    UWorld* World = GetWorld();
    if (!World) return;

    ADDMiniGameStateBase* GS =
        World->GetGameState<ADDMiniGameStateBase>();

    if (!GS)
    {
        World->GetTimerManager().SetTimerForNextTick(
            this,
            &UDDMiniGameReadyWidget::BindToGameState
        );
        return;
    }

    // 중복 방지
    GS->OnMiniGameReadyStateChanged.RemoveAll(this);

    GS->OnMiniGameReadyStateChanged.AddDynamic(
        this,
        &UDDMiniGameReadyWidget::HandleReadyStateChanged
    );

    // 초기 동기화
    HandleReadyStateChanged(
        GS->GetReadyPlayerCount(),
        GS->GetTotalParticipantCount()
    );
}



void UDDMiniGameReadyWidget::HandleReadyStateChanged(int32 Ready, int32 Total)
{
    SetReadyCount(Ready, Total);

    if (Total > 0 && Ready >= Total)
    {
        RemoveFromParent(); // Ready UI 자동 종료
    }
}