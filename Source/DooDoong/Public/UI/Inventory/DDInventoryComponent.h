
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DDInventoryComponent.generated.h"

class UDDItemUseButtonWidget;
struct FItemTableRow;
class UDataTable;
class UDDInventoryWidget;
class ADDBasePlayerController;
class UDDInventoryBase;

USTRUCT(BlueprintType)
struct FInventoryItemData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FName ItemName;
	UPROPERTY()
	int32 Count;
	
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class DOODOONG_API UDDInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDDInventoryComponent();

public:
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	/** 주현 : 인벤토리 Open이 Private이라 한 번 감싸서 호출가능하게 요청하는 함수 */
	void RequestOpenInventory();

	/** 주현 : 인벤토리 Close도 Private이라 한 번 감싸서 호출가능하게 요청하는 함수 */
	void RequestCloseInventory();
	
	/* 서버 관련 함수 */
	
	UFUNCTION(Server, Reliable)
	void ServerRPCUseItem(const FName& ItemSlotName);
	
	UFUNCTION(Server, Reliable)
	void ServerRPCAddItem(FName ItemName);
	
	void InitializeInventoryData();
	void InitializeInventoryUI();
	
	/* 레플리케이티드가 된다면 서버에서 획득삭제시 클라에 동기화 */
	FItemTableRow* GetItemData(FName RowName) const;
	

	/* 클라이언트 전용 복제 변수 */
	UPROPERTY(ReplicatedUsing=OnRep_ClientInventoryData)
	TArray<FInventoryItemData> InventoryItemDatas;
	
	UFUNCTION()
	void OnRep_ClientInventoryData();
	UFUNCTION()
	FName AddRandomItem();

protected:
	UPROPERTY(EditDefaultsOnly, Category= "Inventory|Data")
	TObjectPtr<UDataTable> ItemDataTable;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "OwningController")
	TWeakObjectPtr<ADDBasePlayerController> OwningController;
};
