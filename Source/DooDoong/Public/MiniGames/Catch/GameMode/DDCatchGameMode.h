#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "DDCatchGameMode.generated.h"

UCLASS()
class DOODOONG_API ADDCatchGameMode : public ADDMiniGameModeBase
{
	GENERATED_BODY()
	
public:
	ADDCatchGameMode();
	
	virtual void StartMiniGame() override;
};
