#pragma once

#include "CoreMinimal.h"
#include "ItemBase/GA_TargetingItemBase.h"
#include "GA_Portal.generated.h"

UCLASS()
class DOODOONG_API UGA_Portal : public UGA_TargetingItemBase
{
	GENERATED_BODY()
	virtual bool ExecuteTargetingItem(AActor* TargetActor) override;
};
