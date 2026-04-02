#include "AbilitySystem/Attributes/DDBaseAttributeSet.h"

#include "Net/UnrealNetwork.h"

UDDBaseAttributeSet::UDDBaseAttributeSet()
{
	InitHealth(30.0f);
	InitMaxHealth(30.0f);
}

void UDDBaseAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UDDBaseAttributeSet, Health);
	DOREPLIFETIME(UDDBaseAttributeSet, MaxHealth);
}

void UDDBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

void UDDBaseAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UDDBaseAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDDBaseAttributeSet, Health, OldHealth);
}

void UDDBaseAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDDBaseAttributeSet, MaxHealth, OldMaxHealth);
}
