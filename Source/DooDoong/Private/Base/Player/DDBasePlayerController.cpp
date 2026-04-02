#include "Base/Player/DDBasePlayerController.h"

#include "AbilitySystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/DDAbilitySystemComponent.h"
#include "Input/DDInputComponent.h"
#include "System/DDGameplayTags.h"

ADDBasePlayerController::ADDBasePlayerController()
{
}

void ADDBasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultIMC, 0);
	}
}

void ADDBasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UDDInputComponent* DDInputComponent =  CastChecked<UDDInputComponent>(InputComponent);
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

void ADDBasePlayerController::SetInputConfig(UDDInputConfig* NewConfig)
{
	if (NewConfig)
	{
		InputConfig = NewConfig; 
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
