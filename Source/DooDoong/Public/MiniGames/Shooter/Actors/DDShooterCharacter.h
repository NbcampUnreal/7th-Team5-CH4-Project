#pragma once

#include "CoreMinimal.h"
#include "Base/Character/DDBaseCharacter.h"
#include "DDShooterCharacter.generated.h"

UCLASS()
class DOODOONG_API ADDShooterCharacter : public ADDBaseCharacter
{
	GENERATED_BODY()

public:
	ADDShooterCharacter();

protected:
	virtual void BeginPlay() override;
};
