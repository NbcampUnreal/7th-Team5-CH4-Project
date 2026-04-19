#pragma once

#include "CoreMinimal.h"
#include "DDItemDataTypes.h"
#include "ItemPayloadObject.generated.h"

UCLASS()
class UItemPayloadObject : public UObject
{
	GENERATED_BODY()

public:
	FItemTableRow ItemRow;
};