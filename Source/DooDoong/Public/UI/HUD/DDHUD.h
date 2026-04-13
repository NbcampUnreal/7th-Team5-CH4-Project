#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameplayTagContainer.h"
#include "UI/TestLogicManager.h"
#include "UI/UIConfigData.h"   // ⭐ 이거 필수
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
	UFUNCTION()
	void HandleMiniGameStateChanged(FGameplayTag NewState);

	void ApplyInitialHealth(float Value);
	void OnHealthChanged(const FOnAttributeChangeData& Data);

	UFUNCTION()
	void HandleReadyStateChanged(int32 ReadyCount, int32 TotalCount);

	void ShowWidget(TSubclassOf<UUserWidget> WidgetClass);
	void HideMainWidget();

	void ShowMiniGameReadyUI();
	void HideMiniGameReadyUI();

	void BindToGameState();
	void BindToPlayerState();

	void OnCoinChanged(const FOnAttributeChangeData& Data);


	void TryBindGAS();

	UFUNCTION()
	void InitHUDDelayed();


	void OnGASReady();

	void UpdateHealth(float Value);
	void UpdateGold(int32 Value);

	UPROPERTY()
	UBaseGameWidget* MainWidget;

	UPROPERTY()
	TObjectPtr<UDDMiniGameReadyWidget> MiniGameReadyWidget;

protected:


	UPROPERTY()
	UTestLogicManager* TestLogicManager;

	UPROPERTY()
	TObjectPtr<UUIConfigData> UIConfig;

	UPROPERTY()
	TObjectPtr<UUserWidget> MainWidgetInstance;



	UPROPERTY()
	TObjectPtr<UDDMiniGameManager> MiniGameManager;

	UPROPERTY()
	FGameplayTag LastState;
};