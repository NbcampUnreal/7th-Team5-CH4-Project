#include "BoardGame/DDDiceActor.h"


ADDDiceActor::ADDDiceActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// 네트워크에서 복제 가능하게
	bReplicates = true; 
	SetReplicateMovement(true);
}

void ADDDiceActor::StartRoll(int32 InValue)
{
	TargetValue = InValue;
	bRolling = true;
	RollingTime = 0.f;
}

void ADDDiceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bRolling) return;

	RollingTime += DeltaTime;

	// 🎯 굴러가는 연출
	AddActorLocalRotation(FRotator(720 * DeltaTime, 1080 * DeltaTime, 0));

	// 🎯 1.5초 후 멈춤
	if (RollingTime > 1.5f)
	{
		bRolling = false;

		SetActorRotation(GetRotationForDice(TargetValue));
		
		OnRollFinished.Broadcast();
	}
}

FRotator ADDDiceActor::GetRotationForDice(int32 Value)
{
	switch (Value)
	{
	case 1: return FRotator(0, 0, 0);
	case 2: return FRotator(90, 0, 0);
	case 3: return FRotator(0, 0, 90);
	case 4: return FRotator(0, 0, -90);
	case 5: return FRotator(-90, 0, 0);
	case 6: return FRotator(0, 0, 180);
	default: return FRotator::ZeroRotator;
	}
}
