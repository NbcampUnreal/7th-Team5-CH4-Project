#include "BoardGame/Game/DDBoardGameState.h"
#include "Net/UnrealNetwork.h"

ADDBoardGameState::ADDBoardGameState()
{
	bReplicates = true;
}

void ADDBoardGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADDBoardGameState, MaxStateTimer);
	DOREPLIFETIME(ADDBoardGameState, MaxTrophy);
	DOREPLIFETIME(ADDBoardGameState, MaxRound);
	DOREPLIFETIME(ADDBoardGameState, StateTimer);
	DOREPLIFETIME(ADDBoardGameState, CurrentRound);
}

void ADDBoardGameState::OnRep_StateTimer()
{
	OnStateTimerChanged.Broadcast(StateTimer);
}

void ADDBoardGameState::OnRep_CurrentRound()
{
	OnRoundChanged.Broadcast(CurrentRound);
}
