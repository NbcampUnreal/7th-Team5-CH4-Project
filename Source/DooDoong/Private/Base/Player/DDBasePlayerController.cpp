#include "Base/Player/DDBasePlayerController.h"

#include "AbilitySystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/DDAbilitySystemComponent.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "Input/DDInputComponent.h"
#include "UI/MiniGameUI/MiniGameUIWrapper.h"
#include "System/DDGameplayTags.h"
#include "System/MiniGame/DDMiniGameManager.h"

ADDBasePlayerController::ADDBasePlayerController()
{
    MiniGameUI = nullptr;
}

void ADDBasePlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(DefaultIMC, 0);
    }

    // MiniGameUI 생성
    if (!MiniGameUI)
    {
        MiniGameUI = NewObject<UMiniGameUIWrapper>(this, UMiniGameUIWrapper::StaticClass());
        if (MiniGameUI)
        {
            MiniGameUI->AddToRoot(); // GC 방지
        }
    }
}

void ADDBasePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UDDInputComponent* DDInputComponent = CastChecked<UDDInputComponent>(InputComponent);
    if (!IsValid(DDInputComponent)) return;

    if (InputConfig)
    {
        // Move
        DDInputComponent->BindNativeAction(InputConfig, DDGameplayTags::Input_Native_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, true);

        // Look
        DDInputComponent->BindNativeAction(InputConfig, DDGameplayTags::Input_Native_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look, true);

        // Abilities
        TArray<uint32> BindHandles;
        DDInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityPressed, &ThisClass::Input_AbilityReleased, BindHandles);
    }
}

void ADDBasePlayerController::SetInputMappingContext(UInputMappingContext* NewIMC)
{
    if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (DefaultIMC)
        {
            Subsystem->RemoveMappingContext(DefaultIMC);
        }

        DefaultIMC = NewIMC;
        if (DefaultIMC)
        {
            Subsystem->AddMappingContext(DefaultIMC, 0);
        }
    }
}

void ADDBasePlayerController::Client_ApplyState_Implementation(FGameplayTag StateTag)
{
    if (StateTag.MatchesTag(DDGameplayTags::State_BoardGame_LobbyWaiting) ||
        StateTag.MatchesTag(DDGameplayTags::State_BoardGame_LobbyStarting))
    {
        SetInputMappingContext(DefaultIMC);
    }
    else if (StateTag.MatchesTag(DDGameplayTags::State_BoardGame_Init) ||
        StateTag.MatchesTag(DDGameplayTags::State_BoardGame_PlayerTurn) ||
        StateTag.MatchesTag(DDGameplayTags::State_BoardGame_RoundEnd))
    {
        SetInputMappingContext(BoardGameIMC);
    }
}

void ADDBasePlayerController::Client_ApplyInput_Implementation(UInputMappingContext* NewIMC)
{
    SetInputMappingContext(NewIMC);
}

void ADDBasePlayerController::Server_SetMiniGameReady_Implementation(bool bReady)
{
    if (ADDMiniGameModeBase* MiniGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ADDMiniGameModeBase>() : nullptr)
    {
        MiniGameMode->SetPlayerReady(PlayerState, bReady);
    }
}

void ADDBasePlayerController::Input_Move(const FInputActionValue& Value)
{
    if (APawn* ControlledPawn = GetPawn())
    {
        const FVector2D MoveValue = Value.Get<FVector2D>();
        const FRotator MoveRotation(0.f, GetControlRotation().Yaw, 0.f);

        const FVector ForwardDirection = MoveRotation.RotateVector(FVector::ForwardVector);
        const FVector RightDirection = MoveRotation.RotateVector(FVector::RightVector);

        ControlledPawn->AddMovementInput(ForwardDirection, MoveValue.Y);
        ControlledPawn->AddMovementInput(RightDirection, MoveValue.X);
    }
}

void ADDBasePlayerController::Input_Look(const FInputActionValue& Value)
{
    const FVector2D LookValue = Value.Get<FVector2D>();
    AddYawInput(LookValue.X);
    AddPitchInput(LookValue.Y);
}

void ADDBasePlayerController::Input_AbilityPressed(FGameplayTag InputTag)
{
    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetPawn()))
    {
        if (UDDAbilitySystemComponent* DDASC = Cast<UDDAbilitySystemComponent>(ASI->GetAbilitySystemComponent()))
        {
            DDASC->AbilityInputTagPressed(InputTag);
        }
    }
}

void ADDBasePlayerController::Input_AbilityReleased(FGameplayTag InputTag)
{
    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetPawn()))
    {
        if (UDDAbilitySystemComponent* DDASC = Cast<UDDAbilitySystemComponent>(ASI->GetAbilitySystemComponent()))
        {
            DDASC->AbilityInputTagReleased(InputTag);
        }
    }
}

// MiniGame UI
void ADDBasePlayerController::Client_OpenReadyUI_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("Client_OpenReadyUI 실행됨")); // 추가

    if (!ReadyWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ReadyWidgetClass NULL"));
        return;
    }

    ReadyWidget = CreateWidget<UUserWidget>(this, ReadyWidgetClass);

    if (!ReadyWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateWidget 실패"));
        return;
    }

    ReadyWidget->AddToViewport();
}

void ADDBasePlayerController::Client_CloseReadyUI_Implementation()
{
    if (MiniGameUI)
    {
        MiniGameUI->CloseReadyUI(this);
    }
}




void ADDBasePlayerController::Server_StartRandomMiniGame_Implementation()
{
    if (GetGameInstance() == nullptr) return;

    UDDMiniGameManager* Manager = GetGameInstance()->GetSubsystem<UDDMiniGameManager>();
    if (Manager == nullptr) return;

    TArray<APlayerState*> Players;

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (PC && PC->PlayerState)
        {
            Players.Add(PC->PlayerState);
        }
    }

    Manager->RequestStartRandomMiniGame(Players);
}
