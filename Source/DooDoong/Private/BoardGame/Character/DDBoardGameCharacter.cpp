#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/DDDiceActor.h"
#include "BoardGame/DDTile.h"
#include "Common/DDLogManager.h"
#include "System/DDGameplayTags.h"

ADDBoardGameCharacter::ADDBoardGameCharacter()
{
}

void ADDBoardGameCharacter::InitLocation()
{
	ADDBasePlayerState* BasePlayerState = GetPlayerState<ADDBasePlayerState>();
	if (!BasePlayerState) return;
	if (BasePlayerState->CurrentTile)
	{
		LOG_CYS(Warning, TEXT("[BC]SetActorLocation"));
		SetActorLocation(BasePlayerState->CurrentTile->GetStandLocation(this));
	}
}

void ADDBoardGameCharacter::MoveToLocation(FVector TargetLocation)
{
	if (bIsMoving) return;

	UWorld* World = GetWorld();
	if (!World) return;

	MoveStartLocation = GetActorLocation();
	MoveTargetLocation = TargetLocation;

	MoveElapsedTime = 0.f;
	MoveDuration = 0.3f;

	bIsMoving = true;

	World->GetTimerManager().SetTimer(
		MoveTimerHandle,
		this,
		&ADDBoardGameCharacter::UpdateMove,
		World->GetDeltaSeconds(),
		true
	);
}

void ADDBoardGameCharacter::UpdateMove()
{
	UWorld* World = GetWorld();
	if (!World) return;

	float DeltaTime = World->GetDeltaSeconds();
	MoveElapsedTime += DeltaTime;

	float Alpha = MoveElapsedTime / MoveDuration;
	Alpha = FMath::Clamp(Alpha, 0.f, 1.f);

	Alpha = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 2.f);

	FVector LerpPos = FMath::Lerp(MoveStartLocation, MoveTargetLocation, Alpha);

	float Height = 100.f;
	float ZOffset = Height * FMath::Sin(Alpha * PI);

	LerpPos.Z += ZOffset;

	SetActorLocation(LerpPos);

	if (Alpha >= 1.f)
	{
		SetActorLocation(MoveTargetLocation);

		bIsMoving = false;

		World->GetTimerManager().ClearTimer(MoveTimerHandle);

		// 주사위 파괴
		if (IsValid(Dice))
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

void ADDBoardGameCharacter::PlayDice(int32 DiceValue)
{
	if (!HasAuthority())
	{
		return;
	}
	
	UWorld* World = GetWorld();
	if (!World) return;

	// 서버에서 Dice 스폰, 클라이언트는 복제된 actor을 봄
	if (!DiceClass)
	{
		LOG_CYS(Error, TEXT("DiceClass is NULL"));
		return;
	}

	FVector Loc = GetMesh()->GetSocketLocation(TEXT("head"));
	Loc.Z += HeadOffset;
	Dice = World->SpawnActor<ADDDiceActor>(
		DiceClass,
		Loc,
		FRotator::ZeroRotator
	);

	if (Dice)
	{
		LOG_CYS(Warning, TEXT("DiceSpawn: %s"), *Dice->GetName());
		Dice->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::KeepWorldTransform,
			TEXT("head")
		);

		Dice->StartRoll(DiceValue);
	}

	Multicast_PlayDiceAnimation();
}

void ADDBoardGameCharacter::Multicast_PlayDiceAnimation_Implementation()
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
}

void ADDBoardGameCharacter::Multicast_ShowTileContentAboveHead_Implementation(FGameplayTag TileTag)
{
	if (!GetMesh()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FVector Loc = GetMesh()->GetSocketLocation(TEXT("head"));
	Loc.Z += HeadOffset;

	// 기존 액터 있으면 제거 (중복 방지)
	if (EventActor && IsValid(EventActor))
	{
		EventActor->Destroy();
		EventActor = nullptr;
	}
	TSubclassOf<AActor> SpawnClass = nullptr;

	if (TileTag.MatchesTag(DDGameplayTags::Tile_Ability_Coin))
	{
		// 코인 
		LOG_CYS(Error, TEXT("코인 머리 위!"));
		SpawnClass = CoinClass;
	}
	else if (TileTag.MatchesTag(DDGameplayTags::Tile_Ability_Item))
	{
		LOG_CYS(Error, TEXT("아이템 머리 위!"));
		// 택 1
		// 1. 아이템 랜덤 반환: 아이템 스폰해서 오버랩 이벤트로 아이템 획득 
		// 2. 가방보여주고 아이템이 그 안에 있는 느낌: 아이템은 따로 UI로 알려줌 
	}
	else if (TileTag.MatchesTag(DDGameplayTags::Tile_Ability_Move))
	{
		// 무브
		LOG_CYS(Error, TEXT("무브 머리 위!"));
		SpawnClass = MoveClass;
	}

	if (!SpawnClass)
	{
		LOG_CYS(Error, TEXT("No Spawn Class"));
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());

	FRotator SpawnRot = FRotator::ZeroRotator;

	if (PC)
	{
		FVector CamLoc;
		FRotator CamRot;
		PC->GetPlayerViewPoint(CamLoc, CamRot);

		SpawnRot = (CamLoc - Loc).Rotation();

		// 위아래 기울기 제거 (자연스럽게)
		SpawnRot.Pitch = 0.f;
		SpawnRot.Roll = 0.f;
	}

	EventActor = World->SpawnActor<AActor>(
		SpawnClass,
		Loc,
		SpawnRot
	);

	if (!EventActor)
	{
		LOG_CYS(Error, TEXT("EventActor 스폰 실패"));
		return;
	}

	EventActor->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::KeepWorldTransform,
		TEXT("head")
	);
	LOG_CYS(Warning, TEXT("Spawn: %s"), *EventActor->GetName());

	// (임시) 2초 후 파괴
	World->GetTimerManager().SetTimer(
		EventDestroyTimerHandle,
		[this]()
		{
			if (EventActor && IsValid(EventActor))
			{
				EventActor->Destroy();
				EventActor = nullptr;
			}
		},
		2.0f,
		false
	);
}
