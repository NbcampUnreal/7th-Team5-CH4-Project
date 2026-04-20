// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDInventoryWidget.generated.h"


class ADDBasePlayerController;
class UDDInventoryComponent;
class UCanvasPanel;
class UDDInvenGridSlot;



UCLASS()
class DOODOONG_API UDDInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void UpdateGrid();
	
protected:
	void GenerateGrid();
	
	void InitInventory(UDDInventoryComponent* Inventory);
	
private:
	/* 인벤토리 그리기에 필요한 데이터 */
	
	
	UPROPERTY(EditDefaultsOnly, category="Inventory|Data")
	TObjectPtr<UDataTable> ItemDataTable;
	
	/* 인벤토리 그리드 슬롯 */
	UPROPERTY()
	TArray<UDDInvenGridSlot*> GridSlots;
	
	/* 인벤토리 컴포넌트 */
	UPROPERTY()
	TObjectPtr<UDDInventoryComponent> InventoryComponent;

	/* 인벤토리 슬롯 클래스 */
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSubclassOf<UDDInvenGridSlot> GridSlotClass;
	
	/* 캔버스 패널 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	
	/* 인벤토리 슬롯개수*/
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	int32 Rows;
	
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	int32 Columns;
	
};
