#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingWidget.generated.h"

class UButton;
class UComboBoxString;
class UTextBlock;
class USlider;

struct FScreenResolution
{
	int32 Width;
	int32 Height;
	FString ToString() const { return FString::Printf(TEXT("%d x %d"), Width, Height); }
};

UCLASS()
class DOODOONG_API USettingWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct();
	
protected:
	UFUNCTION()
	void OnMasterVolumeChanged(float Value);
	
	UFUNCTION()
	void OnBGMVolumeChanged(float Value);
	
	UFUNCTION()
	void OnSFXVolumeChanged(float Value);
	
	UFUNCTION()
	void OnInterfaceVolumeChanged(float Value);
	
	UFUNCTION()
	void OnApplyClicked();
	
	UFUNCTION()
	void OnBackClicked();
	
protected:
	void UpdateVolumeText(UTextBlock* Text, float Value);
	void InitResolutionOption();
	void InitWindowModeOption();
	void LoadCurrentSettings();
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> MasterVolumeSlider;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> BGMVolumeSlider;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> SFXVolumeSlider;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> InterfaceVolumeSlider;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MasterVolumeText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> BGMVolumeText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SFXVolumeText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InterfaceVolumeText;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ResolutionComboBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> WindowModeComboBox;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ApplyButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;
	
	TArray<FScreenResolution> Resolutions;
};
