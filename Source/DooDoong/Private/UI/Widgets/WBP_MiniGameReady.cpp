// WBP_MiniGameReady.cpp
#include "UI/Widgets/WBP_MiniGameReady.h"
#include "Base/Player/DDBasePlayerController.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UWBP_MiniGameReady::NativeConstruct()
{
    Super::NativeConstruct();

    if (ReadyButton)
    {
        ReadyButton->OnClicked.AddDynamic(this, &UWBP_MiniGameReady::OnReadyButtonClicked);
    }
}

void UWBP_MiniGameReady::OnReadyButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("ReadyButton Clicked"));

    if (APlayerController* PC = GetOwningPlayer())
    {
        if (ADDBasePlayerController* BasePC = Cast<ADDBasePlayerController>(PC))
        {
            UE_LOG(LogTemp, Warning, TEXT("Cast Success"));
            BasePC->Server_SetMiniGameReady(true);
            UE_LOG(LogTemp, Warning, TEXT("Server_SetMiniGameReady called"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Cast Failed"));
        }
    }
}