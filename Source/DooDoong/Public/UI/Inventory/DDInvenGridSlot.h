// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDInvenGridSlot.generated.h"

class UImage;
class UButton;

UCLASS()
class DOODOONG_API UDDInvenGridSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetItemInfo(UTexture2D* Image) const;
	
protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void UseItem();
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BT_UseItem;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_ItemIcon;
	
	int32 ItemID;
	
	/* 버튼동작확인 임시 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "GridSlot|Bool", meta=(AllowPrivateAccess = true))
	bool bCanUse;
};
