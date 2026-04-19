#include "Lobby/Game/DDLobbyGameState.h"
#include "Common/DDLogManager.h"
#include "Net/UnrealNetwork.h" 


ADDLobbyGameState::ADDLobbyGameState()
{
}

void ADDLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ADDLobbyGameState, ReadyCount);
	DOREPLIFETIME(ADDLobbyGameState, PlayerCount);
	DOREPLIFETIME(ADDLobbyGameState, CountDown);
}

void ADDLobbyGameState::OnRep_PlayerCount()
{
	LOG_KMS(Warning, TEXT("PlayerCount : %d"), PlayerCount);
	OnPlayerCountChanged.Broadcast(PlayerCount);
}

void ADDLobbyGameState::OnRep_ReadyCount()
{
	LOG_KMS(Warning, TEXT("ReadyCount : %d"), ReadyCount);
	OnReadyCountChanged.Broadcast(ReadyCount);
}

void ADDLobbyGameState::OnRep_CountDown()
{
	OnCountDownChanged.Broadcast(CountDown);
}


