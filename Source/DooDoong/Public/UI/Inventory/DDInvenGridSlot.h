#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDInvenGridSlot.generated.h"

class UDDInventoryWidget;
struct FViewItemData;
class UTextBlock;
class UImage;
class UButton;
class UDDItemUseButtonWidget;

UCLASS()
class DOODOONG_API UDDInvenGridSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	void SetItemInfo(const FViewItemData& ViewItemData);
	
	void UpdateItemInfo(const FViewItemData& ViewItemData);
	
	UFUNCTION()
	void UseItem();
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BT_ClickItem;
	
private:
	/* 그리드슬롯 그리는 UI변수 */
	
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_ItemIcon;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_ItemCount;
	
	/* 그리드슬롯 클릭시 그려지는 UI위젯 변수 */
	UPROPERTY()
	TObjectPtr<UDDItemUseButtonWidget> ItemUseButtonWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UDDItemUseButtonWidget> ItemUseButtonWidgetClass;
	
	/* 그리드슬롯 그리기 내부 데이터 */
	FName ItemName;
	int32 ItemCount;
	bool bCanUse;
};
