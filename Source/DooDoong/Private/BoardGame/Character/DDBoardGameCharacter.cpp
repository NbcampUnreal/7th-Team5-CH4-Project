#include "BoardGame/Character/DDBoardGameCharacter.h"

#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/DDTile.h"
#include "Common/DDLogManager.h"

ADDBoardGameCharacter::ADDBoardGameCharacter()
{
}

void ADDBoardGameCharacter::InitLocation()
{
	ADDBasePlayerState* BasePlayerState = GetPlayerState<ADDBasePlayerState>();
	if (!BasePlayerState) return;
	if (BasePlayerState->CurrentTile)
	{
		LOG_CYS(Warning,TEXT("[BC]SetActorLocation"));
		SetActorLocation(BasePlayerState->CurrentTile->GetStandLocation(this));
	}
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

		OnMoveFinished.Broadcast(); // 이동 완료 태스크에 알림
	}
}
