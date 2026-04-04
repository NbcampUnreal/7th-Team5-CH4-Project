#pragma once

#include "CoreMinimal.h"
#include "Base/Game/DDGameModeBase.h"
#include "DDTestBoardGameMode.generated.h"


UCLASS()
class DOODOONG_API ADDTestBoardGameMode : public ADDGameModeBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
