#include "AbilitySystem/Attributes/DDPointSet.h"

#include "GameplayEffectExtension.h"
#include "BoardGame/Game/DDBoardGameMode.h"
#include "Net/Core/Connection/NetConnectionFaultRecoveryBase.h"

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
		OnCoinChanged.Broadcast(NewValue);
	}
	else if (Attribute == GetTrophyAttribute())
	{
		OnTrophyChanged.Broadcast(NewValue);
	}
	
	if (ADDBoardGameMode* BM = Cast<ADDBoardGameMode>(GetWorld()->GetAuthGameMode()))
	{
	    BM->RequestRankUpdate();
	}
	
}

void UDDPointSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if (Data.EvaluatedData.Attribute == GetCoinAttribute())
	{
		float Delta = Data.EvaluatedData.Magnitude;

		if (Delta < 0.f) // 코인 감소
		{
			LastCoinLose = -Delta; // 양수로 저장
		}
	}
}

void UDDPointSet::OnRep_Trophy(const FGameplayAttributeData& OldTrophy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDDPointSet, Trophy, OldTrophy);
	OnTrophyChanged.Broadcast(FMath::FloorToInt32(GetTrophy()));
}

void UDDPointSet::OnRep_Coin(const FGameplayAttributeData& OldCoin)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDDPointSet, Coin, OldCoin);
	OnCoinChanged.Broadcast(FMath::FloorToInt32(GetCoin()));
}
