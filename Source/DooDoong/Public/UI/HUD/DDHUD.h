#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DDHUD.generated.h"

UCLASS()
class DOODOONG_API ADDHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	// UI 교체용 (나중 확장)
	void ShowWidget(TSubclassOf<class UUserWidget> WidgetClass);

protected:
	// 로비 UI
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> LobbyWidgetClass;

	// 기본 게임 UI
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> BaseGameWidgetClass;

	// 현재 위젯
	UPROPERTY()
	UUserWidget* MainWidgetInstance;
};