// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDInventory.generated.h"

class UCanvasPanel;
class UDDInvenGridSlot;

UCLASS()
class DOODOONG_API UDDInventory : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	
private:
	void ConstructGrid();
	
	UPROPERTY()
	TArray<TObjectPtr<UDDInvenGridSlot>> GridSlots;
	
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSubclassOf<UDDInvenGridSlot> GridSlotClass;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	int32 Rows;
	
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	int32 Columns;
	
};
