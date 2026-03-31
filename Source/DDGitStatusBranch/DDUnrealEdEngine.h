#pragma once

#include "CoreMinimal.h"
#include "Editor/UnrealEdEngine.h"
#include "DDUnrealEdEngine.generated.h"

UCLASS()
class DDGITSTATUSBRANCH_API UDDUnrealEdEngine : public UUnrealEdEngine
{
	GENERATED_BODY()
	
	virtual void Init(IEngineLoop* InEngineLoop) override;
};
