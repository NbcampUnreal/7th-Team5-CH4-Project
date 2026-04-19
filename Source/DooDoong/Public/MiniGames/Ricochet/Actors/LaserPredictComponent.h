#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "LaserPredictComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DOODOONG_API ULaserPredictComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    ULaserPredictComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void StartPreview();
    void StopPreview();

protected:

    void UpdatePreview();
    void BuildPath(TArray<FVector>& OutPoints);
    void BuildSpline(const TArray<FVector>& Points);

protected:

    UPROPERTY()
    USplineComponent* Spline;

    TArray<USplineMeshComponent*> Meshes;

    bool bIsActive = false;

    float MaxDistance = 8000.f;
    int MaxBounces = 5;
};