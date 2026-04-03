#include "BoardGame/Character/DDBoardGameCharacter.h"

ADDBoardGameCharacter::ADDBoardGameCharacter()
{
	
}
void ADDBoardGameCharacter::InitLocation(ADDTile* TargetTile)
{
	if (!TargetTile) return;
	SetActorLocation(TargetTile->GetStandLocation(this));
}
void ADDBoardGameCharacter::MoveToLocation(FVector TargetLocation)
{
	if (bIsMoving) return;

	MoveStartLocation = GetActorLocation();
	MoveTargetLocation = TargetLocation;

	MoveElapsedTime = 0.f;
	MoveDuration = 0.3f;

	bIsMoving = true;

	GetWorld()->GetTimerManager().SetTimer(
		MoveTimerHandle,
		this,
		&ADDBoardGameCharacter::UpdateMove,
		0.016f, // 약 60fps
		true
	);
}

void ADDBoardGameCharacter::UpdateMove()
{
	MoveElapsedTime += 0.016f;

	float Alpha = MoveElapsedTime / MoveDuration;
	Alpha = FMath::Clamp(Alpha, 0.f, 1.f);

	Alpha = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 2.f);

	FVector LerpPos = FMath::Lerp(MoveStartLocation, MoveTargetLocation, Alpha);

	float Height = 100.f;
	float ZOffset = Height * 4 * Alpha * (1 - Alpha);

	LerpPos.Z += ZOffset;

	SetActorLocation(LerpPos);

	if (Alpha >= 1.f)
	{
		SetActorLocation(MoveTargetLocation);

		bIsMoving = false;

		GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);

		OnMoveFinished.Broadcast();
	}
}
