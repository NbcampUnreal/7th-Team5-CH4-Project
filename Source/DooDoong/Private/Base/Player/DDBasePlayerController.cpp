#include "Base/Player/DDBasePlayerController.h"

#include "AbilitySystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/DDAbilitySystemComponent.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "BoardGame/DDSelectableTileActor.h"
#include "BoardGame/Abilities/DDMoveTileStepTask.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "Common/DDLogManager.h"
#include "Input/DDInputComponent.h"
#include "System/DDGameplayTags.h"
#include "UI/GameUIManager.h"
#include "System/MiniGame/DDMiniGameManager.h"

ADDBasePlayerController::ADDBasePlayerController()
{
}

void ADDBasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	UIManager = NewObject<UGameUIManager>(this);
	UIManager->Initialize(this, UIConfig);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultIMC, 0);
	}
}

void ADDBasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UDDInputComponent* DDInputComponent = CastChecked<UDDInputComponent>(InputComponent);
	if (!IsValid(DDInputComponent)) return;

	if (InputConfig)
	{
		// Bind Move InputAction
		DDInputComponent->BindNativeAction(
			InputConfig,
			DDGameplayTags::Input_Native_Move,
			ETriggerEvent::Triggered,
			this,
			&ThisClass::Input_Move,
			true
		);

		// Bind Look InputAction
		DDInputComponent->BindNativeAction(
			InputConfig,
			DDGameplayTags::Input_Native_Look,
			ETriggerEvent::Triggered,
			this,
			&ThisClass::Input_Look,
			true
		);

		// Bind Click InputAction
		DDInputComponent->BindNativeAction(
			InputConfig,
			DDGameplayTags::Input_Native_Click,
			ETriggerEvent::Started,
			this,
			&ThisClass::OnMouseClick,
			true
		);
		
		// Bind Abilities InputActions
		TArray<uint32> BindHandles;
		DDInputComponent->BindAbilityActions(
			InputConfig,
			this,
			&ThisClass::Input_AbilityPressed,
			&ThisClass::Input_AbilityReleased,
			BindHandles
		);
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
	UE_LOG(LogTemp, Error, TEXT("Server_SetMiniGameReady CALLED"));
	ADDMiniGameModeBase* MiniGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ADDMiniGameModeBase>() : nullptr;
	if (MiniGameMode == nullptr)
	{
		return;
	}

	MiniGameMode->SetPlayerReady(PlayerState, bReady);
}

void ADDBasePlayerController::Call_SetReady()
{
	UE_LOG(LogTemp, Warning, TEXT("Call_SetReady called"));
	Server_SetMiniGameReady(true);
}

void ADDBasePlayerController::OnMouseClick()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (!Hit.GetActor()) return;

	ADDSelectableTileActor* SelectableTileActor = Cast<ADDSelectableTileActor>(Hit.GetActor());
	if (!SelectableTileActor) return;

	LOG_CYS(Warning, TEXT("Tile Clicked (Client)"));
	LOG_CYS(Warning, TEXT("Hit: %s"), *GetNameSafe(Hit.GetActor()));
	// 서버로 전달
	Server_SelectTile(SelectableTileActor);
}

void ADDBasePlayerController::Server_SelectTile_Implementation(ADDSelectableTileActor* SelectableTileActor)
{
	if (!SelectableTileActor) return;

	ADDBoardGameCharacter* BoardCharacter = Cast<ADDBoardGameCharacter>(GetPawn());
	if (!BoardCharacter) return;
	if (!BoardCharacter->CurrentMoveTask) return;

	if (BoardCharacter->CurrentMoveTask)
	{
		BoardCharacter->CurrentMoveTask->SelectNextTile(SelectableTileActor->TargetTile);
	}
}

void ADDBasePlayerController::Input_Move(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	const FVector2D MoveValue = Value.Get<FVector2D>();
	const FRotator MoveRotation(0.f, GetControlRotation().Yaw, 0.f);

	const FVector ForwardDirection = MoveRotation.RotateVector(FVector::ForwardVector);
	const FVector RightDirection = MoveRotation.RotateVector(FVector::RightVector);

	ControlledPawn->AddMovementInput(ForwardDirection, MoveValue.Y);
	ControlledPawn->AddMovementInput(RightDirection, MoveValue.X);
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
