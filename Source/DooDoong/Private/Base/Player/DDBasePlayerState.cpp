#include "Base/Player/DDBasePlayerState.h"
#include "AbilitySystem//DDAbilitySystemComponent.h"

ADDBasePlayerState::ADDBasePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UDDAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	
	HealthSet = CreateDefaultSubobject<UDDHealthSet>(TEXT("HealthSet"));

	SetNetUpdateFrequency(100.0f);
}

UAbilitySystemComponent* ADDBasePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent; 
}
