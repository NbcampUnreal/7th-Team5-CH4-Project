#include "AbilitySystem/Attributes/DDMovementSet.h"

UDDMovementSet::UDDMovementSet()
{
	InitMoveSpeed(400.f); 
	InitJumpSpeed(420.f);
}

void UDDMovementSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UDDMovementSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UDDMovementSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UDDMovementSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDDMovementSet, MoveSpeed, OldMoveSpeed);
}

void UDDMovementSet::OnRep_JumpSpeed(const FGameplayAttributeData& OldJumpSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDDMovementSet, JumpSpeed, OldJumpSpeed);
}
