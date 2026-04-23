#include "MiniGames/Batter/Character/DDBatterCharacter.h"
#include "EnhancedInputComponent.h"
#include "MiniGames/Batter/GameMode/DDBatterGameMode.h"
#include "Net/UnrealNetwork.h"


ADDBatterCharacter::ADDBatterCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADDBatterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ADDBatterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(IA_Bat, ETriggerEvent::Started, this, &ADDBatterCharacter::OnPressSpace);
		EnhancedInput->BindAction(IA_Bat,ETriggerEvent::Completed, this, &ADDBatterCharacter::OnReleaseSpace);
	}
}

void ADDBatterCharacter::Server_AddScore_Implementation()
{
	ADDBatterGameMode* GM = GetWorld()->GetAuthGameMode<ADDBatterGameMode>();
	if (!GM) return;

	APlayerState* PS = GetPlayerState();
	if (!PS) return;

	GM->AddScore(PS, 1);
}

void ADDBatterCharacter::OnPressSpace()
{
	UE_LOG(LogTemp, Warning, TEXT("Space Pressed!"));
	
	Server_AddScore();
	
	Server_SetPress(true);
}

void ADDBatterCharacter::OnReleaseSpace()
{
	Server_SetPress(false);
}

void ADDBatterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADDBatterCharacter, bPressing);
}

void ADDBatterCharacter::Server_SetPress_Implementation(bool bNewPress)
{
	bPressing = bNewPress;
}
