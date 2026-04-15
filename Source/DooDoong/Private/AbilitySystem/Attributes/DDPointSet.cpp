#include "AbilitySystem/Attributes/DDPointSet.h"

UDDPointSet::UDDPointSet()
{
	InitCoin(0.f);
	InitTrophy(0.f);
}

void UDDPointSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UDDPointSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetCoinAttribute() || Attribute == GetTrophyAttribute()) 
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
}

void UDDPointSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetCoinAttribute())
	{
		OnPointChanged.Broadcast(NewValue);
	}
	else if (Attribute == GetTrophyAttribute())
	{
		OnTrophyChanged.Broadcast(NewValue);	
	}
	
}

void UDDPointSet::OnRep_Trophy(const FGameplayAttributeData& OldTrophy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDDPointSet, Trophy, OldTrophy);
}

void UDDPointSet::OnRep_Coin(const FGameplayAttributeData& OldCoin)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDDPointSet, Coin, OldCoin);
}
