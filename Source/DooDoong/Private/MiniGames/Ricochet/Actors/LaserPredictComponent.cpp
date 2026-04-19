#include "MiniGames/Ricochet/Actors/LaserPredictComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

ULaserPredictComponent::ULaserPredictComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULaserPredictComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULaserPredictComponent::StartPreview()
{
	bIsHolding = true;
	CurrentLength = 0.f;
}

void ULaserPredictComponent::StopPreview()
{
	bIsHolding = false;
	CurrentLength = 0.f;
}

void ULaserPredictComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsHolding) return;

	CurrentLength += DeltaTime * GrowthSpeed;
	CurrentLength = FMath::Clamp(CurrentLength, 0.f, MaxDistance);

	UpdatePreview();
}




void ULaserPredictComponent::BuildPath(TArray<FVector>& OutPoints)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	int32 X, Y;
	PC->GetViewportSize(X, Y);

	FVector Start;
	FVector Dir;

	PC->DeprojectScreenPositionToWorld(X * 0.5f, Y * 0.5f, Start, Dir);

	float RemainingDistance = MaxDistance;

	OutPoints.Add(Start);

	for (int i = 0; i < MaxBounces; i++)
	{
		FHitResult Hit;
		FVector End = Start + Dir * RemainingDistance;

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			Hit,
			Start,
			End,
			ECC_Visibility
		);

		if (bHit)
		{
			OutPoints.Add(Hit.ImpactPoint);

			RemainingDistance -= FVector::Dist(Start, Hit.ImpactPoint);

			// 반사 공식
			Dir = Dir - 2.f * FVector::DotProduct(Dir, Hit.ImpactNormal) * Hit.ImpactNormal;
			Dir.Normalize();

			Start = Hit.ImpactPoint + Dir * 2.f;
		}
		else
		{
			OutPoints.Add(End);
			break;
		}
	}
}






void ULaserPredictComponent::UpdatePreview()
{
	TArray<FVector> FullPoints;
	BuildPath(FullPoints);

	if (FullPoints.Num() < 2) return;

	TArray<FVector> VisiblePoints;

	float Remaining = CurrentLength;

	VisiblePoints.Add(FullPoints[0]);

	for (int i = 0; i < FullPoints.Num() - 1; i++)
	{
		FVector A = FullPoints[i];
		FVector B = FullPoints[i + 1];

		float SegmentLen = FVector::Distance(A, B);

		if (Remaining > SegmentLen)
		{
			VisiblePoints.Add(B);
			Remaining -= SegmentLen;
		}
		else
		{
			FVector Dir = (B - A).GetSafeNormal();
			FVector Partial = A + Dir * Remaining;

			VisiblePoints.Add(Partial);
			break;
		}
	}

	DrawDashedLine(VisiblePoints);
}





void ULaserPredictComponent::DrawDashedLine(const TArray<FVector>& Points)
{
	float DashLength = 80.f;
	float Gap = 40.f;

	for (int i = 0; i < Points.Num() - 1; i++)
	{
		FVector Start = Points[i];
		FVector End = Points[i + 1];

		FVector Dir = (End - Start).GetSafeNormal();
		float Length = FVector::Distance(Start, End);

		float Drawn = 0.f;

		while (Drawn < Length)
		{
			float Segment = FMath::Min(DashLength, Length - Drawn);

			FVector DashStart = Start + Dir * Drawn;
			FVector DashEnd = DashStart + Dir * Segment;

			DrawDebugLine(
				GetWorld(),
				DashStart,
				DashEnd,
				FColor::White,
				false,
				0.05f,
				0,
				2.f
			);

			Drawn += DashLength + Gap;
		}
	}
}