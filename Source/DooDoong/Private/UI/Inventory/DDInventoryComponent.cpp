
#include "UI/Inventory/DDInventoryComponent.h"
#include "UI/Inventory/DDInventory.h"

#include "Base/Player/DDBasePlayerController.h"
#include "UI/Inventory/DDInventoryBase.h"


UDDInventoryComponent::UDDInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UDDInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	ConstructInventory();	
}

void UDDInventoryComponent::ConstructInventory()
{
	OwningController = Cast<ADDBasePlayerController>(GetOwner());
	checkf(OwningController.IsValid(), TEXT("플레이어의 인벤토리가 유효하지않습니다."));
	if (!OwningController->IsLocalController()) return;
	
	InventoryWidget = CreateWidget<UDDInventory>(OwningController.Get(), InventoryWidgetClass);
	InventoryWidget->AddToViewport();
	
}
