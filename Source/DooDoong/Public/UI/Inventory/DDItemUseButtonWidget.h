// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDItemUseButtonWidget.generated.h"

struct FItemTableRow;
class UMultiLineEditableTextBox;
class UDDInventoryComponent;
class UButton;

UCLASS()
class DOODOONG_API UDDItemUseButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void UseButton();
	
	UFUNCTION()
	void CancelButton();
	
	UFUNCTION()
	void InitializeGridSlotData(const FName SlotItemName);
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BT_Use;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BT_Cancel;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UMultiLineEditableTextBox> ET_ItemDescription;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UMultiLineEditableTextBox> ET_ItemName;
	
	FName CurrentItemSlotName;
};
