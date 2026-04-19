#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LaserPredictComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DOODOONG_API ULaserPredictComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULaserPredictComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void StartPreview();
	void StopPreview();

private:
	void UpdatePreview();
	void BuildPath(TArray<FVector>& OutPoints);
	void DrawDashedLine(const TArray<FVector>& Points);

private:
	bool bIsHolding = false;

	float CurrentLength = 0.f;

	UPROPERTY(EditAnywhere)
	float GrowthSpeed = 2000.f;

	UPROPERTY(EditAnywhere)
	float MaxDistance = 5000.f;

	UPROPERTY(EditAnywhere)
	int32 MaxBounces = 5;
};