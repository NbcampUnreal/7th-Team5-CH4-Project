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
	
	void DrawPopup(FGameplayTag PopupTag);
	void DrawPopupForDuration(FGameplayTag PopupTag, float Duration);
	
	void HidePopup(FGameplayTag PopupTag);
	void HideAllPopups();

	template<typename T>
	T* GetGameLayer() const { return Cast<T>(GameLayerWidget); }

	template<typename T>
	T* GetOverlay() const { return Cast<T>(OverlayWidget); }

	void SetUIConfig(UDDUIConfig* InUIConfig);
	
	void DrawErrorMessage(const FString& ErrorMessage, float Duration);
	
private:
	UUserWidget* CreateUIWidget(TSubclassOf<UUserWidget> WidgetClass);
	
	UPROPERTY()
	TObjectPtr<UDDUIConfig> UIConfig;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> GameLayerWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> OverlayWidget;
	
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UUserWidget>> PopupWidgets;
};
