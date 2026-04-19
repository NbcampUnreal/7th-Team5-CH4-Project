
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DDInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

class UItemActionComponent;
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
	int32 ItemCount;
	UPROPERTY()
	bool bCanUse;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;
};

USTRUCT(BlueprintType)
struct FViewItemData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FName ViewItemName;
	UPROPERTY()
	int32 ViewItemCount;
	UPROPERTY()
	bool bCanUse;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;
	
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
	
	/* 인벤토리 새로고침 */
	void RefreshInventory();
	
	/* 서버 관련 함수 */
	
	UFUNCTION(Server, Reliable)
	void Server_UseItem(const FName& ItemSlotName);
	
	UFUNCTION(Server, Reliable)
	void Server_AddItem(FName ItemName);
	
	UFUNCTION(Server, Reliable)
	void Server_InitializeInventoryData();
	
	/* 레플리케이티드가 된다면 서버에서 획득삭제시 클라에 동기화 */
	FItemTableRow* GetItemData(FName RowName) const;
	
	/* 원본데이터 */
	UPROPERTY()
	TArray<FInventoryItemData> InventoryItemDatas;
	
	/* 복제데이터 */
	UPROPERTY(ReplicatedUsing=OnRep_ClientViewItemData)
	TArray<FViewItemData> ViewItemDatas;
	
	UFUNCTION()
	void OnRep_ClientViewItemData();
	
	FOnInventoryChanged OnInventoryChanged;
	
	UFUNCTION()
	FName AddRandomItem();

protected:
	UPROPERTY(EditDefaultsOnly, Category= "Inventory|Data")
	TObjectPtr<UDataTable> ItemDataTable;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "OwningController")
	TObjectPtr<ADDBasePlayerController> OwningController;
};
