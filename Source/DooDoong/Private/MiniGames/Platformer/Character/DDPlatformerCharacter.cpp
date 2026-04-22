

#include "MiniGames/Platformer/Character/DDPlatformerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


ADDPlatformerCharacter::ADDPlatformerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bInheritPitch = true;
	SpringArmComp->bInheritYaw = true;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->TargetArmLength = 300.0f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;
}

void ADDPlatformerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADDPlatformerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADDPlatformerCharacter::Server_ReturnToSavePoint_Implementation()
{
	SetActorLocation(SavePointLocation);
}

void ADDPlatformerCharacter::Server_SetSavePointLocation_Implementation(const FVector& UpDateSavePoint)
{
	SavePointLocation = UpDateSavePoint;
}



