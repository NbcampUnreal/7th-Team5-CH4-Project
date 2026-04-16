#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "GameplayTagContainer.h"
#include "DDUIManagerSubsystem.generated.h"


class UDDUIConfig;

UCLASS()
class DOODOONG_API UDDUIManagerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	void SwitchGameLayer(FGameplayTag StateTag);

	void ShowOverlay(TSubclassOf<UUserWidget> WidgetClass);
	void HideOverlay();
	
	void TogglePopup(FGameplayTag PopupTag);
	void HidePopup();

	template<typename T>
	T* GetGameLayer() const { return Cast<T>(GameLayerWidget); }

	template<typename T>
	T* GetOverlay() const { return Cast<T>(OverlayWidget); }

	void SetUIConfig(UDDUIConfig* InUIConfig);
	
private:
	UUserWidget* CreateUIWidget(TSubclassOf<UUserWidget> WidgetClass);

	UPROPERTY()
	TObjectPtr<UUserWidget> GameLayerWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> OverlayWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> PopupWidget;
	
	UPROPERTY()
	TObjectPtr<UDDUIConfig> UIConfig;
};
