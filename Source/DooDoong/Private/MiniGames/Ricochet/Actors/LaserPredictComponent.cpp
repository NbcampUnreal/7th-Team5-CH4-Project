

#include "MiniGames/Ricochet/Actors/LaserPredictComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

ULaserPredictComponent::ULaserPredictComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void ULaserPredictComponent::BeginPlay()
{
    Super::BeginPlay();

    Spline = NewObject<USplineComponent>(this);
    Spline->RegisterComponent();

    if (AActor* Owner = GetOwner())
    {
        Spline->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
    }
}

void ULaserPredictComponent::StartPreview()
{
    bIsActive = true;
}

void ULaserPredictComponent::StopPreview()
{
    bIsActive = false;
}

void ULaserPredictComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsActive)
    {
        UpdatePreview();
    }
}


void ULaserPredictComponent::UpdatePreview()
{
    TArray<FVector> Points;
    BuildPath(Points);
    BuildSpline(Points);
}


void ULaserPredictComponent::BuildPath(TArray<FVector>& OutPoints)
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    FVector Start;
    FVector Dir;

    int32 X, Y;
    PC->GetViewportSize(X, Y);

    PC->DeprojectScreenPositionToWorld(
        X * 0.5f,
        Y * 0.5f,
        Start,
        Dir
    );

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

            // 반사
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




void ULaserPredictComponent::BuildSpline(const TArray<FVector>& Points)
{
    Spline->ClearSplinePoints(false);

    for (int i = 0; i < Points.Num(); i++)
    {
        Spline->AddSplinePoint(Points[i], ESplineCoordinateSpace::World, false);
    }

    Spline->UpdateSpline();
}