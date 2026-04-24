#include "Base/Game/DDGameStateBase.h"

ADDGameStateBase::ADDGameStateBase()
{
	bReplicates = true;
}

void ADDGameStateBase::BeginPlay()
{
	Super::BeginPlay();
}

void ADDGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
