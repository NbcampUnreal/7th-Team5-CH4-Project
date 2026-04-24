#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDDiceActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDiceRollFinished);

UCLASS()
class DOODOONG_API ADDDiceActor : public AActor
{
	GENERATED_BODY()
public:
	ADDDiceActor();
	virtual void BeginPlay() override;
	void StartRoll(int32 InValue);

protected:
	virtual void Tick(float DeltaTime) override;
	FRotator GetRotationForDice(int32 Value);

private:
	int32 TargetValue;
	float RollingTime = 0.f;
	bool bRolling = false;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnDiceRollFinished OnRollFinished;
};
