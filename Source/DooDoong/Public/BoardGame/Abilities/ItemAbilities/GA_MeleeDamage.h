#pragma once

#include "CoreMinimal.h"
#include "ItemBase/GA_RangeItemBase.h"
#include "GA_MeleeDamage.generated.h"

UCLASS()
class DOODOONG_API UGA_MeleeDamage : public UGA_RangeItemBase
{
	GENERATED_BODY()
protected:
	virtual void OnConfirm(FGameplayEventData Payload) override;
};
