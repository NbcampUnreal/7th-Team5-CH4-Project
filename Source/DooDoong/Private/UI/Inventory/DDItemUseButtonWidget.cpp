// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/DDItemUseButtonWidget.h"

#include "Components/Button.h"

void UDDItemUseButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BT_Use)
	{
		BT_Use->OnClicked.AddDynamic(this, &UDDItemUseButtonWidget::UseButton);
	}
		
	if (BT_Cancel)
	{
		BT_Cancel->OnClicked.AddDynamic(this, &UDDItemUseButtonWidget::CancelButton);
	}
}

void UDDItemUseButtonWidget::UseButton()
{
	this->RemoveFromParent();
}

void UDDItemUseButtonWidget::CancelButton()
{
	this->RemoveFromParent();
}
