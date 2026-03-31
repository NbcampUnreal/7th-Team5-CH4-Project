#include "Base/Character/DDBaseCharacter.h"
#include "AbilitySystem/DDAbilitySystemComponent.h"

ADDBaseCharacter::ADDBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UDDAbilitySystemComponent>("AbilitySystemComponent");
}


void ADDBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void ADDBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->ProcessAbilityInput(DeltaTime, false);
	}
}


void ADDBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		
		AbilitySystemComponent->GiveDefaultAbilities(); 
	}
}

UAbilitySystemComponent* ADDBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}







