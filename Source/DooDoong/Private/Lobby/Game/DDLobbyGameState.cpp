#include "Lobby/Game/DDLobbyGameState.h"
#include "Common/DDLogManager.h"
#include "Net/UnrealNetwork.h"
#include "System/DDSoundManager.h"


ADDLobbyGameState::ADDLobbyGameState()
{
}

void ADDLobbyGameState::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (UDDSoundManager* SoundManager = UDDSoundManager::Get(GetWorld()))
	{
		SoundManager->PlayBGM("BGM_Lobby", 0.5f);
	}
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

void ADDLobbyGameState::Multicast_StopLobbyBGM_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (UDDSoundManager* SoundManager = UDDSoundManager::Get(GetWorld()))
	{
		SoundManager->StopBGM(0.5f);
	}
}
