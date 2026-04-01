#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DDMiniGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DOODOONG_API ADDMiniGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ADDMiniGameModeBase();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
