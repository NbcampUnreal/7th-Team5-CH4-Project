#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "DDBatterGameMode.generated.h"

UCLASS()
class DOODOONG_API ADDBatterGameMode : public ADDMiniGameModeBase
{
	GENERATED_BODY()
	
public:
	ADDBatterGameMode();
	
	virtual void StartMiniGame() override;
	
	virtual void FinishMiniGame() override;
};
