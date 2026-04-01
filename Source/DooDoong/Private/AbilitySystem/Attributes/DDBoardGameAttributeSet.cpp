#include "AbilitySystem/Attributes/DDBoardGameAttributeSet.h"

UDDBoardGameAttributeSet::UDDBoardGameAttributeSet()
{
	Health = 30.f;
	MaxHealth = 30.f; 
	
	Coin = 0.f;
	Trophy = 0.f; 
}

void UDDBoardGameAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UDDBoardGameAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UDDBoardGameAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}
