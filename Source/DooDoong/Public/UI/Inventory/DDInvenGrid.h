// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDInvenGrid.generated.h"

class UCanvasPanel;
class UDDInvenGridSlot;

UCLASS()
class DOODOONG_API UDDInvenGrid : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	
private:
	void ConstructGrid();
	
	UPROPERTY()
	TArray<TObjectPtr<UDDInvenGridSlot>> GridSlots;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UDDInvenGridSlot> GridSlotClass;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Rows;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Columns;
	
};
