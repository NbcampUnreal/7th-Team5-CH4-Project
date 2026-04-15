#include "BoardGame/Game/DDBoardGameState.h"

#include "Net/UnrealNetwork.h"

void ADDBoardGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ADDBoardGameState, MaxStateTimer);
	DOREPLIFETIME(ADDBoardGameState, StateTimer);
	DOREPLIFETIME(ADDBoardGameState, MaxTrophy);
	DOREPLIFETIME(ADDBoardGameState, MaxRound);
}
