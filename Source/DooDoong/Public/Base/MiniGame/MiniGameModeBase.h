#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MiniGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DOODOONG_API AMiniGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMiniGameModeBase();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
