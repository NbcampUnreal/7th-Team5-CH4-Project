
#include "UI/Inventory/DDInventoryComponent.h"

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
	checkf(OwningController.IsValid(), TEXT("인벤토리가 플레이어 컨트롤러 소유여야합니다."));
	if (!OwningController->IsLocalController()) return;
	
	InventoryMenu = CreateWidget<UDDInventoryBase>(OwningController.Get(), InventoryMenuClass);
	InventoryMenu->AddToViewport();
	
}
