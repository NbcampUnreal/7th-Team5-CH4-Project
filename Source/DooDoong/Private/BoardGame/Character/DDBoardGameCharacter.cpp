#include "BoardGame/Character/DDBoardGameCharacter.h"

#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/DDDiceActor.h"
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

		// 주사위 파괴
		if (Dice && Dice->IsValidLowLevel())
		{
			// 소켓에서 Detach
			Dice->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

			LOG_CYS(Error, TEXT("Dice->DetachFromActor: %s"), *Dice->GetName());
			// 삭제
			Dice->Destroy();
			Dice = nullptr;
		}
		OnMoveFinished.Broadcast(); // 이동 완료 태스크에 알림
	}
}

void ADDBoardGameCharacter::Multicast_PlayDiceAnimation_Implementation(int32 DiceValue)
{
	if (!DiceMontage)
	{
		LOG_CYS(Error, TEXT("DiceMontage is NULL"));
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(DiceMontage);
	}
	if (HasAuthority())
	{
		// 서버에서 Dice 스폰, 클라이언트는 복제된 actor을 봄
		if (!DiceClass)
		{
			LOG_CYS(Error, TEXT("DiceClass is NULL"));
			return;
		}

		FVector Loc = GetMesh()->GetSocketLocation(TEXT("head"));
		Loc.Z+=70;
		Dice = GetWorld()->SpawnActor<ADDDiceActor>(
			DiceClass,
			Loc,
			FRotator::ZeroRotator
		);

		LOG_CYS(Error, TEXT("DiceSpawn: %s"), *Dice->GetName());
		if (Dice)
		{
			Dice->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules::KeepWorldTransform,
				TEXT("head")
			);

			Dice->StartRoll(DiceValue);
		}
	}
}
