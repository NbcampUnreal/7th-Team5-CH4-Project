#include "Base/Game/DDGameStateBase.h"
#include "Net/UnrealNetwork.h"

void ADDGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADDGameStateBase, MatchState);
}
