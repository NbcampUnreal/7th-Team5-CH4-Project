#pragma once

#include "CoreMinimal.h"
#include "ItemBase/GA_RangeItemBase.h"
#include "GA_Magnet.generated.h"

UCLASS()
class DOODOONG_API UGA_Magnet : public UGA_RangeItemBase
{
	GENERATED_BODY()

protected:
	virtual void OnConfirm(FGameplayEventData Payload) override;
};
