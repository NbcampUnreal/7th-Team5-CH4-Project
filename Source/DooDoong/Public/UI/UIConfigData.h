// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UIConfigData.generated.h"

UCLASS()
class UUIConfigData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MiniGameReadyWidget;

	// ✅ 추가
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MiniGameHUDWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> BaseGameWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LobbyWidget;
};
