#include "Base/Character/DDBaseCharacter.h"
#include "AbilitySystem/DDAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Base/Player/DDBasePlayerState.h"
#include "AbilitySystem/Attributes/DDHealthSet.h"
#include "AbilitySystem/Attributes/DDMovementSet.h"
#include "Components/CapsuleComponent.h"

ADDBaseCharacter::ADDBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

UAbilitySystemComponent* ADDBaseCharacter::GetAbilitySystemComponent() const
{
	if (AbilitySystemComponent)	return AbilitySystemComponent;
	
	if (const ADDBasePlayerState* PS = GetPlayerState<ADDBasePlayerState>())
	{
		return PS->GetAbilitySystemComponent();
	}
	
	return nullptr; 
}

void ADDBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ADDBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled() && AbilitySystemComponent)
	{
		AbilitySystemComponent->ProcessAbilityInput(DeltaTime, false);
	}
}

void ADDBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitializeAbilitySystem(); 
	
	BindAttributeDelegates();
	
	if (HasAuthority() && AbilitySystemComponent)
	{
		AbilitySystemComponent->GiveAbilities(DefaultAbilitySet);
	}
	
	if (ADDBasePlayerState* PS = GetPlayerState<ADDBasePlayerState>())
    {
        PS->UpdateCharacterVisuals();
    }
}

void ADDBaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	InitializeAbilitySystem();
	
	BindAttributeDelegates();
	
	if (ADDBasePlayerState* PS = GetPlayerState<ADDBasePlayerState>())
    {
        PS->UpdateCharacterVisuals();
    }
}

void ADDBaseCharacter::InitializeAbilitySystem()
{
	ADDBasePlayerState* PS = GetPlayerState<ADDBasePlayerState>();
	if (!PS) return; 
	
	AbilitySystemComponent = Cast<UDDAbilitySystemComponent>(PS->GetAbilitySystemComponent());
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	}
	
	HealthSet = PS->GetHealthSet();
	MovementSet = PS->GetMovementSet();
}

void ADDBaseCharacter::BindAttributeDelegates()
{
	if (!AbilitySystemComponent || !MovementSet) return;
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MovementSet->GetMoveSpeedAttribute()).RemoveAll(this);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MovementSet->GetJumpSpeedAttribute()).RemoveAll(this);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MovementSet->GetMoveSpeedAttribute())
	.AddUObject(this, &ThisClass::OnWalkSpeedChanged);
	
	FOnAttributeChangeData WalkSpeedData;
	WalkSpeedData.NewValue = MovementSet->GetMoveSpeed();
	WalkSpeedData.OldValue = MovementSet->GetMoveSpeed();
    
	OnWalkSpeedChanged(WalkSpeedData);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MovementSet->GetJumpSpeedAttribute())
	.AddUObject(this, &ThisClass::OnJumpSpeedChanged);
	
	FOnAttributeChangeData JumpSpeedData;
	JumpSpeedData.NewValue = MovementSet->GetJumpSpeed();
	JumpSpeedData.OldValue = MovementSet->GetJumpSpeed();
	
	OnJumpSpeedChanged(JumpSpeedData);
	
}

void ADDBaseCharacter::OnWalkSpeedChanged(const FOnAttributeChangeData& Data) 
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->MaxWalkSpeed = Data.NewValue;
	}
}

void ADDBaseCharacter::OnJumpSpeedChanged(const FOnAttributeChangeData& Data)
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->JumpZVelocity = Data.NewValue;
	}
}

void ADDBaseCharacter::MultiCast_HandleRagDoll_Implementation()
{
	EnableRagDoll();
}

void ADDBaseCharacter::EnableRagDoll()
{
	UE_LOG(LogTemp,Warning,TEXT("Character RagDoll"));
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
		MovementComp->DisableMovement();
	}
	
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	}
	
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetAnimInstanceClass(nullptr);
		MeshComp->SetAnimationMode(EAnimationMode::AnimationCustomMode);
		MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
		MeshComp->SetAllBodiesSimulatePhysics(true);
		MeshComp->SetSimulatePhysics(true);
		
		MeshComp->WakeAllRigidBodies();
		
		MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		MeshComp->SetSimulatePhysics(true);
	}
}

UDDHealthSet* ADDBaseCharacter::GetHealthSet() const
{
	if (HealthSet) return HealthSet;
	
	if (ADDBasePlayerState* PS = GetPlayerState<ADDBasePlayerState>())
	{
		return PS->GetHealthSet();
	}
	
	return nullptr;
}

UDDMovementSet* ADDBaseCharacter::GetMovementSet() const
{
	if (MovementSet) return MovementSet;
	
	if (ADDBasePlayerState* PS = GetPlayerState<ADDBasePlayerState>())
	{
		return PS->GetMovementSet();
	}
	
	return nullptr;
}

void ADDBaseCharacter::ApplyColorFromPlayerState(FLinearColor NewColor)
{
	if (!CachedColorMaterial && GetMesh())
	{
		CachedColorMaterial = GetMesh()->CreateDynamicMaterialInstance(0);
	}

	if (CachedColorMaterial)
	{
		CachedColorMaterial->SetVectorParameterValue(FName("PlayerColor"), NewColor);
	}
}