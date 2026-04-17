#include "BoardGame/Abilities/ItemAbilities/GA_Range.h"

#include "BoardGame/Character/DDBoardGameCharacter.h"


void UGA_Range::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	GetWorld()->GetTimerManager().SetTimer(
		UpdateTimer,
		this,
		&UGA_Range::UpdateTargets,
		0.1f,
		true
		);
}

void UGA_Range::ActivateAbilityFromEvent(const FGameplayEventData& EventData)
{
	const FItemTableRow* ItemData = static_cast<const FItemTableRow*>(EventData.OptionalObject);

	// 저장
	CachedItemData = *ItemData;
}

void UGA_Range::UpdateTargets()
{
	ADDBoardGameCharacter* MyChar = Cast<ADDBoardGameCharacter>(GetAvatarActorFromActorInfo());
	if (!MyChar) return;

	FVector Origin = MyChar->GetActorLocation();
	FVector Forward = MyChar->GetActorForwardVector();

	CachedTargets.Empty();

	// 1. 디버그 부채꼴
	DrawDebugCone(
		GetWorld(),
		Origin,
		Forward,
		Radius,
		FMath::DegreesToRadians(AngleDeg * 0.5f),
		FMath::DegreesToRadians(AngleDeg * 0.5f),
		20,
		FColor::Green,
		false,
		0.1f
	);

	// 2. 플레이어 탐색
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APawn* Pawn = It->Get()->GetPawn();
		if (!Pawn || Pawn == MyChar) continue;

		if (IsInFanRange(Origin, Forward, Pawn->GetActorLocation()))
		{
			CachedTargets.Add(Pawn);

			// 타겟 표시
			DrawDebugSphere(GetWorld(), Pawn->GetActorLocation(), 30.f, 8, FColor::Red, false, 0.1f);
		}
	}
}

bool UGA_Range::IsInFanRange(const FVector& Origin, const FVector& Forward, const FVector& TargetLoc)
{
	FVector ToTarget = TargetLoc - Origin;

	float Distance = ToTarget.Size();
	if (Distance > Radius) return false;

	ToTarget.Normalize();

	float Dot = FVector::DotProduct(Forward, ToTarget);
	float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));

	return Angle <= AngleDeg * 0.5f;
}
