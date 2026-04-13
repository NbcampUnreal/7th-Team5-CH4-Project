#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameplayTagContainer.h"

#include "UI/TestLogicManager.h"
#include "UI/UIConfigData.h"
#include "AbilitySystemComponent.h"

#include "Base/Player/DDBasePlayerState.h"
#include "AbilitySystem/Attributes/DDPointSet.h"

#include "UI/Widgets/BaseGameWidget.h"
#include "UI/Widgets/DDMiniGameReadyWidget.h"

#include "DDHUD.generated.h"

class UUserWidget;
class UDDMiniGameManager;
class UBaseGameWidget;

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
	// State / Binding
	// =========================
	void BindToGameState();
	void BindToPlayerState();

	UFUNCTION()
	void HandleMiniGameStateChanged(FGameplayTag NewState);

	UFUNCTION()
	void HandleReadyStateChanged(int32 ReadyCount, int32 TotalCount);

	// =========================
	// GAS
	// =========================
	void TryBindGAS();
	void OnGASReady();

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnCoinChanged(const FOnAttributeChangeData& Data);

	void ApplyInitialHealth(float Value);

	// =========================
	// UI Update
	// =========================
	void UpdateHealth(float Value);
	void UpdateGold(int32 Value);

	UFUNCTION()
	void InitHUDDelayed();

public:
	// =========================
	// UI State
	// =========================
	UPROPERTY()
	TObjectPtr<UUserWidget> MainWidgetInstance;

	UPROPERTY()
	TObjectPtr<UBaseGameWidget> MainWidget;

	UPROPERTY()
	TObjectPtr<UDDMiniGameReadyWidget> MiniGameReadyWidget;

	UPROPERTY()
	TObjectPtr<UDDMiniGameManager> MiniGameManager;

	UPROPERTY()
	TObjectPtr<UUIConfigData> UIConfig;

	UPROPERTY()
	UTestLogicManager* TestLogicManager;

	FGameplayTag LastState;
};