// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDInvenGridSlot.generated.h"

class UImage;

UCLASS()
class DOODOONG_API UDDInvenGridSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetImage(UImage* Image);
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_ItemIcon;
	
	int32 ItemID;
	
	bool bOwned;
};
