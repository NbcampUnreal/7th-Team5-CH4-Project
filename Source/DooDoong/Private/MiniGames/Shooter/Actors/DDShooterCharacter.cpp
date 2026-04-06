#include "MiniGames/Shooter/Actors/DDShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ADDShooterCharacter::ADDShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bUseControllerRotationYaw = true;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->TargetArmLength = 200.f;
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bInheritPitch = true;
	SpringArmComp->bInheritYaw = true;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->bDoCollisionTest = false;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;
	
	MuzzleComp = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleComp"));
	MuzzleComp->SetupAttachment(GetMesh());
}

void ADDShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
}