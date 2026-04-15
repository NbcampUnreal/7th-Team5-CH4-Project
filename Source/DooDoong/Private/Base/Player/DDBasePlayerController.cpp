#include "Base/Player/DDBasePlayerController.h"

#include "AbilitySystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/DDAbilitySystemComponent.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/DDSelectableTileActor.h"
#include "BoardGame/Abilities/DDMoveTileStepTask.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "BoardGame/Character/Components/ItemActionComponent.h"
#include "Common/DDLogManager.h"
#include "Input/DDInputComponent.h"
#include "System/DDGameplayTags.h"
#include "UI/Inventory/DDInventoryComponent.h"
#include "System/MiniGame/DDMiniGameManager.h"

ADDBasePlayerController::ADDBasePlayerController()
{
	InventoryComponent = CreateDefaultSubobject<UDDInventoryComponent>(TEXT("InventoryComponent"));
}

void ADDBasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	FInputModeGameAndUI Mode;
	SetInputMode(Mode);
	bShowMouseCursor = true;

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
		
		// Bind Inven InputActor
		DDInputComponent->BindNativeAction(
			InputConfig,
			DDGameplayTags::Input_Native_Inventory,
			ETriggerEvent::Started,
			this,
			&ThisClass::ToggleInventoryMenu,
			true
		);

		DDInputComponent->BindNativeAction(
			InputConfig,
			DDGameplayTags::Input_Native_ItemNextTarget,
			ETriggerEvent::Started,
			this,
			&ThisClass::Input_ItemNextTarget,
			true
		);

		DDInputComponent->BindNativeAction(
			InputConfig,
			DDGameplayTags::Input_Native_ItemPreviousTarget,
			ETriggerEvent::Started,
			this,
			&ThisClass::Input_ItemPreviousTarget,
			true
		);

		DDInputComponent->BindNativeAction(
			InputConfig,
			DDGameplayTags::Input_Native_ItemConfirm,
			ETriggerEvent::Started,
			this,
			&ThisClass::Input_ItemConfirm,
			true
		);

		DDInputComponent->BindNativeAction(
			InputConfig,
			DDGameplayTags::Input_Native_ItemCancel,
			ETriggerEvent::Started,
			this,
			&ThisClass::Input_ItemCancel,
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
	ADDMiniGameModeBase* MiniGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ADDMiniGameModeBase>() : nullptr;
	if (MiniGameMode == nullptr)
	{
		return;
	}

	MiniGameMode->SetPlayerReady(PlayerState, bReady);
}

void ADDBasePlayerController::Client_SetMouseCursorVisible_Implementation(bool bVisible)
{
	bShowMouseCursor = bVisible;
    if (bVisible)
    {
        FInputModeGameAndUI Mode;
        SetInputMode(Mode);
    }
    else
    {
        FInputModeGameOnly Mode;
        SetInputMode(Mode);
    }
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

void ADDBasePlayerController::Server_RequestPlayerReady_Implementation()
{
	ADDMiniGameModeBase* GM = GetWorld()->GetAuthGameMode<ADDMiniGameModeBase>();
	if (!IsValid(GM)) return;
	
	ADDBasePlayerState* PS = GetPlayerState<ADDBasePlayerState>();
	if (!IsValid(PS)) return;
	
	GM->SetPlayerReady(PS, true);
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
	if (UItemActionComponent* ItemActionComponent = GetItemActionComponentFromPawn())
	{
		if (ItemActionComponent->IsItemActionActive())
		{
			return;
		}
	}

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

void ADDBasePlayerController::Input_ItemNextTarget()
{
	if (UItemActionComponent* ItemActionComponent = GetItemActionComponentFromPawn())
	{
		if (ItemActionComponent->IsItemActionActive())
		{
			ItemActionComponent->SelectNextTarget();
		}
	}
}

void ADDBasePlayerController::Input_ItemPreviousTarget()
{
	if (UItemActionComponent* ItemActionComponent = GetItemActionComponentFromPawn())
	{
		if (ItemActionComponent->IsItemActionActive())
		{
			ItemActionComponent->SelectPreviousTarget();
		}
	}
}

void ADDBasePlayerController::Input_ItemConfirm()
{
	if (UItemActionComponent* ItemActionComponent = GetItemActionComponentFromPawn())
	{
		if (ItemActionComponent->IsItemActionActive())
		{
			ItemActionComponent->ConfirmItemAction();
		}
	}
}

void ADDBasePlayerController::Input_ItemCancel()
{
	if (UItemActionComponent* ItemActionComponent = GetItemActionComponentFromPawn())
	{
		if (ItemActionComponent->IsItemActionActive())
		{
			ItemActionComponent->CancelItemAction();
		}
	}
}

UItemActionComponent* ADDBasePlayerController::GetItemActionComponentFromPawn() const
{
	const APawn* ControlledPawn = GetPawn();
	return ControlledPawn ? ControlledPawn->FindComponentByClass<UItemActionComponent>() : nullptr;
}

void ADDBasePlayerController::ToggleInventoryMenu()
{
	if (!InventoryComponent) return;
	InventoryComponent->ToggleInventory();
}

