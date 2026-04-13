#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameStateBase.h"
#include "DDCatchGameState.generated.h"

UCLASS()
class DOODOONG_API ADDCatchGameState : public ADDMiniGameStateBase
{
	GENERATED_BODY()

public:
	ADDCatchGameState();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetGameCamera(AActor* CameraActor);
};
