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

    UE_LOG(LogTemp, Warning, TEXT("[Laser] BeginPlay"));

    Spline = NewObject<USplineComponent>(this);
    Spline->RegisterComponent();

    if (AActor* Owner = GetOwner())
    {
        Spline->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
    }
}

void ULaserPredictComponent::StartPreview()
{
    UE_LOG(LogTemp, Warning, TEXT("[Laser] StartPreview"));
    bIsActive = true;
}

void ULaserPredictComponent::StopPreview()
{
    UE_LOG(LogTemp, Warning, TEXT("[Laser] StopPreview"));
    bIsActive = false;

    if (Spline)
    {
        Spline->ClearSplinePoints();
    }
}

void ULaserPredictComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Laser] Tick"));
        UpdatePreview();
    }
}

void ULaserPredictComponent::UpdatePreview()
{
    TArray<FVector> Points;
    BuildPath(Points);

    UE_LOG(LogTemp, Warning, TEXT("[Laser] Points Count: %d"), Points.Num());

    BuildSpline(Points);
}





void ULaserPredictComponent::BuildPath(TArray<FVector>& OutPoints)
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("[Laser] PlayerController NULL"));
        return;
    }

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

    UE_LOG(LogTemp, Warning, TEXT("[Laser] Start: %s"), *Start.ToString());
    UE_LOG(LogTemp, Warning, TEXT("[Laser] Dir: %s"), *Dir.ToString());

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

        //  디버그 라인
        DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.05f, 0, 2.f);

        if (bHit)
        {
            UE_LOG(LogTemp, Warning, TEXT("[Laser] HIT at %s"), *Hit.ImpactPoint.ToString());

            OutPoints.Add(Hit.ImpactPoint);

            RemainingDistance -= FVector::Dist(Start, Hit.ImpactPoint);

            // 반사
            Dir = Dir - 2.f * FVector::DotProduct(Dir, Hit.ImpactNormal) * Hit.ImpactNormal;
            Dir.Normalize();

            Start = Hit.ImpactPoint + Dir * 2.f;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[Laser] NO HIT"));

            OutPoints.Add(End);
            break;
        }
    }
}





void ULaserPredictComponent::BuildSpline(const TArray<FVector>& Points)
{
    if (!Spline)
    {
        UE_LOG(LogTemp, Error, TEXT("[Laser] Spline NULL"));
        return;
    }

    Spline->ClearSplinePoints(false);

    for (int i = 0; i < Points.Num(); i++)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Laser] Add Point %d: %s"), i, *Points[i].ToString());

        Spline->AddSplinePoint(Points[i], ESplineCoordinateSpace::World, false);
    }

    Spline->UpdateSpline();
}