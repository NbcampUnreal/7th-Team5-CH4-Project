// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDInventory.generated.h"

class UDDInventoryComponent;
class UCanvasPanel;
class UDDInvenGridSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS()
class DOODOONG_API UDDInventory : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	
	virtual void NativeConstruct() override;
	
private:
	void GenerationGrid();
	
	void RefreshGrid();
	
	/* 인벤토리 슬롯 */
	UPROPERTY()
	TArray<UDDInvenGridSlot*> GridSlots;
	
	UPROPERTY()
	TObjectPtr<UDDInventoryComponent> InventoryComponent;

	/* 인벤토리 슬롯 클래스 */
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSubclassOf<UDDInvenGridSlot> GridSlotClass;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TArray<UTexture2D*> ItemIcons;
	
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	int32 Rows;
	
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	int32 Columns;
	
};
