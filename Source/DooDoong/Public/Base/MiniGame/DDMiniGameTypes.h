#pragma once

#include "CoreMinimal.h"
#include "DDMiniGameTypes.generated.h"

USTRUCT(BlueprintType)
struct DOODOONG_API FMiniGameSetup
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MiniGameID = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeLimitSeconds = 60.f;
};
