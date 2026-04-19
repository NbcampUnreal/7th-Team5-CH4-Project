#pragma once

#include "CoreMinimal.h"
#include "SoundDataTypes.generated.h"

USTRUCT(BlueprintType)
struct DOODOONG_API FDDSoundDataTableRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoundData")
	TObjectPtr<USoundBase> Sound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoundData")
	float Volume = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoundData")
	float Pitch = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoundData")
	bool bLoop = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoundData")
	bool bAllowMultiple = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoundData")
	float FadeInTime = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoundData")
	float FadeOutTime = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoundData")
	float StartTime = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoundData")
	TObjectPtr<USoundAttenuation> Attenuation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoundData")
	TObjectPtr<USoundConcurrency> Concurrency;
};
