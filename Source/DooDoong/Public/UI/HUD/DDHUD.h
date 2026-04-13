#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameplayTagContainer.h"

#include "UI/UIConfigData.h"
#include "UI/Widgets/BaseGameWidget.h"
#include "UI/Widgets/DDMiniGameReadyWidget.h"

#include "DDHUD.generated.h"

class UUserWidget;
class UDDMiniGameManager;

UCLASS()
class DOODOONG_API ADDHUD : public AHUD
{
	GENERATED_BODY()

public:
	ADDHUD();

protected:
	virtual void BeginPlay() override;

public:
	// =========================
	// Widget Access
	// =========================
	template<typename T>
	T* GetCurrentWidget() const
	{
		return Cast<T>(MainWidgetInstance);
	}

	void ShowWidget(TSubclassOf<UUserWidget> WidgetClass);
	void HideMainWidget();

	// =========================
	// MiniGame UI
	// =========================
	void ShowMiniGameReadyUI();
	void HideMiniGameReadyUI();

	// =========================
	// State
	// =========================
	UPROPERTY()
	FGameplayTag LastState;

	UFUNCTION()
	void HandleMiniGameStateChanged(FGameplayTag NewState);

	// =========================
	// UI Update (필요 시 유지)
	// =========================
	void UpdateHealth(float Value);
	void UpdateGold(int32 Value);

public:
	// =========================
	// UI State
	// =========================
	UPROPERTY()
	TObjectPtr<UUserWidget> MainWidgetInstance;

	UPROPERTY()
	TObjectPtr<UDDMiniGameReadyWidget> MiniGameReadyWidget;

	UPROPERTY()
	TObjectPtr<UDDMiniGameManager> MiniGameManager;

	UPROPERTY()
	TObjectPtr<UUIConfigData> UIConfig;
};