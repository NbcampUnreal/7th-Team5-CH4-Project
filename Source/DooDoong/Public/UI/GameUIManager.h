#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameUIManager.generated.h"

class APlayerController;
class UUserWidget;
class UUIConfigData;

UCLASS()
class UGameUIManager : public UObject
{
	GENERATED_BODY()

public:

	void Initialize(APlayerController* InPC, UUIConfigData* InConfig);

	void ShowMiniGameReadyUI();
	void HideMiniGameReadyUI();

private:

	UPROPERTY()
	TObjectPtr<APlayerController> PC;

	UPROPERTY()
	TObjectPtr<UUIConfigData> UIConfig;

	UPROPERTY()
	TObjectPtr<UUserWidget> ReadyWidget;
};