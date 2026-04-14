// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDInvenGridSlot.generated.h"

struct FItemTableRow;
class UTextBlock;
class UImage;
class UButton;
class UDDItemUseButtonWidget;

UCLASS()
class DOODOONG_API UDDInvenGridSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetItemInfo(FItemTableRow* ItemTableRow);
	
	void UpdateItemInfo(const TMap<FName, int32>& InventoryItemData);
protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void UseItem();
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BT_ClickItem;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_ItemIcon;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_ItemCount;
	
	UPROPERTY()
	TObjectPtr<UDDItemUseButtonWidget> ItemUseButtonWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UDDItemUseButtonWidget> ItemUseButtonWidgetClass;
	
	FName ItemName;
	int32 ItemCount;
	bool bCanUse;
};
