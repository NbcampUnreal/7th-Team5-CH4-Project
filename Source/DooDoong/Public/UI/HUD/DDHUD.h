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

protected:
	// 에디터에서 넣을 위젯 클래스
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> MainWidgetClass;

	// 생성된 위젯 인스턴스
	UPROPERTY()
	UUserWidget* MainWidgetInstance;
};