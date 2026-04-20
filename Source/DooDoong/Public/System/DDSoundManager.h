#pragma once

#include "CoreMinimal.h"
#include "Data/SoundDataTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DDSoundManager.generated.h"

class UAudioComponent;
class UDataTable;
class USceneComponent;

UCLASS()
class DOODOONG_API UDDSoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	/** DDSoundManager Getter */
	static UDDSoundManager* Get(const UObject* WorldContext);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
public:
	UFUNCTION(BlueprintCallable)
	void PlayBGM(FName SoundID, float FadeIn = -1.f);

	UFUNCTION(BlueprintCallable)
	void PlaySound2D(FName SoundID);

	UFUNCTION(BlueprintCallable)
	void PlaySoundAtLocation(FName SoundID, FVector Location);

	UFUNCTION(BlueprintCallable)
	void PlaySoundAttached(FName SoundID, USceneComponent* AttachToComponent, FName SocketName = NAME_None);

	UFUNCTION(BlueprintCallable)
	void StopBGM(float FadeOut = -1.f);

	UFUNCTION(BlueprintCallable)
	void StopSound(FName SoundID, float FadeOut = -1.f);

	UFUNCTION(BlueprintCallable)
	void StopCategory(EDDSoundCategory Category, float FadeOut = -1.f);
	
public:
	UFUNCTION(BlueprintCallable)
	void SetCategoryVolume(EDDSoundCategory Category, float Volume);

	UFUNCTION(BlueprintPure)
	float GetCategoryVolume(EDDSoundCategory Category) const;
	
private:
	const FDDSoundDataTableRow* FindSoundRow(FName SoundID) const;
	float GetFinalVolume(const FDDSoundDataTableRow& SoundRow) const;
	float ResolveFadeTime(float OverrideFadeTime, float DefaultFadeTime) const;

private:
	UPROPERTY()
	TObjectPtr<UDataTable> SoundDataTable;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> CurrentBGM;
	
	UPROPERTY()
	FName CurrentBGMID = NAME_None;
	
	UPROPERTY()
	TMap<EDDSoundCategory, float> CategoryVolumes;
};
