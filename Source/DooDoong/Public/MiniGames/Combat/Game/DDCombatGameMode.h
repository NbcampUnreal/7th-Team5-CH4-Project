#pragma once

#include "CoreMinimal.h"
#include "Base/Game/DDGameModeBase.h"
#include "DDCombatGameMode.generated.h"

UCLASS()
class DOODOONG_API ADDCombatGameMode : public ADDGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void OnCharacterKilled(AActor* Killer, AActor* Victim) override;
};
