#include "Lobby/Game/DDLobbyGameState.h"
#include "Net/UnrealNetwork.h" 


ADDLobbyGameState::ADDLobbyGameState()
{
}

void ADDLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ADDLobbyGameState, ReadyCount);
	DOREPLIFETIME(ADDLobbyGameState, ParticipantCount);
	DOREPLIFETIME(ADDLobbyGameState, CountDown);
}

void ADDLobbyGameState::OnRep_ParticipantCount()
{
}

void ADDLobbyGameState::OnRep_ReadyCount()
{
	OnReadyCountChanged.Broadcast(ReadyCount);
}

void ADDLobbyGameState::OnRep_CountDown()
{
	OnCountDownChanged.Broadcast(CountDown);
}
