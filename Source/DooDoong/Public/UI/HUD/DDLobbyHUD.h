#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DDLobbyHUD.generated.h"

class UUserWidget;

UCLASS()
class ADDLobbyHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	// 컨트롤러의 요청을 받아 UI를 닫는 함수
	void HideLobbyUI();

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> LobbyWidgetInstance;
};
