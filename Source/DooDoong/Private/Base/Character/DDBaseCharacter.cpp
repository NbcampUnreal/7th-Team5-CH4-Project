#include "Base/Character/DDBaseCharacter.h"
#include "AbilitySystem/DDAbilitySystemComponent.h"
#include "Base/Player/DDBasePlayerState.h"
#include "AbilitySystem/Attributes/DDHealthSet.h"
#include "AbilitySystem/Attributes/DDMovementSet.h"

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
	
	if (HasAuthority() && AbilitySystemComponent)
	{
		AbilitySystemComponent->GiveAbilities(DefaultAbilitySet);
	}
}

void ADDBaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	InitializeAbilitySystem();
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
