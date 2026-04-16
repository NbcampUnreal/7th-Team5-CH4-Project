#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/DDItemDataTypes.h"
#include "ItemActionComponent.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum class EItemActionMode : uint8
{
	None,
	Instant,
	Targeting,
	Range
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DOODOONG_API UItemActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemActionComponent();
	
public:
	/** 아이템 사용 시작지점 */
	UFUNCTION(BlueprintCallable, Category = "ItemActionComp")
	void BeginItemAction(const FItemTableRow& ItemRow);
	
	/** 최종적으로 아이템을 사용 == 아이템의 어빌리티를 실행, 아이템 수 감소 */
	UFUNCTION(BlueprintCallable, Category = "ItemActionComp")
	void ConfirmItemAction();
	
	/** 아이템 사용을 취소하고 인벤토리 창을 다시 띄움 */
	UFUNCTION(BlueprintCallable, Category = "ItemActionComp")
	void CancelItemAction();

	/** 다음 타겟 선택 */
	UFUNCTION(BlueprintCallable, Category = "ItemActionComp")
	void SelectNextTarget();

	/** 이전 타겟 선택 */
	UFUNCTION(BlueprintCallable, Category = "ItemActionComp")
	void SelectPreviousTarget();

	/** 아이템 타게팅 이벤트를 전달하는 헬퍼 */
	void SendTargetingInputEvent(FGameplayTag EventTag);

	/** 선택된 타겟으로 타게팅 아이템을 서버에서 확정 */
	void ConfirmTargetingItem(AActor* TargetActor);

	/** 아이템 액션이 진행중인지 확인 : 타게팅할 때 + 범위표시할 때 아이템 액션 중인지 확인하고 수행 */
	UFUNCTION(BlueprintCallable, Category = "ItemActionComp")
	bool IsItemActionActive() const { return CurrentActionMode != EItemActionMode::None; }
	
public:
	/** 타겟 아이템으로 카메라를 이동하는 서버 RPC */
	UFUNCTION(Server, Reliable)
	void Server_FocusItemTarget(AActor* TargetActor);

	/** 즉시사용 아이템 액션 서버 RPC */
	UFUNCTION(Server, Reliable)
	void Server_ConfirmInstantItem(FName ItemID, TSubclassOf<UGameplayAbility> ItemAbility);

	/** 타게팅 아이템 액션 서버 RPC */
	UFUNCTION(Server, Reliable)
	void Server_ConfirmTargetingItem(FName ItemID, TSubclassOf<UGameplayAbility> ItemAbility, AActor* TargetActor);

	/** 범위 아이템 액션 서버 RPC */
	UFUNCTION(Server, Reliable)
	void Server_ConfirmRangeItem(FName ItemID, TSubclassOf<UGameplayAbility> ItemAbility);

protected:
	/** 즉시사용 아이템 액션 */
	void StartInstantAction();
	
	/** 타게팅 아이템 액션 */
	void StartTargetingAction();
	
	/** 범위 아이템 액션 */
	void StartRangeAction();
	
protected:
	/** 서버에서 아이템 Ability를 1회성으로 부여하고 실행 */
	bool TryActivateItemAbility(FName ItemID, TSubclassOf<UGameplayAbility> ItemAbility, AActor* TargetActor);

	/** 취소 시 인벤토리에서 선차감한 아이템 수량을 복구 */
	void RestoreCanceledItem(FName ItemID);
	
	/** 아이템 액션값들을 초기화 */
	void ResetItemAction();
	
protected:
	/** 아이템 액션 상태 태그 추가 */
	void ApplyItemActionTag();
	
	/** 아이템 액션 상태 태그 제거 */
	void RemoveItemActionTag();
	
protected:	
	UPROPERTY()
	EItemActionMode CurrentActionMode = EItemActionMode::None;
	
	UPROPERTY()
	FName ActiveItemID = NAME_None;
	
	UPROPERTY()
	FGameplayTag ActiveItemTag;
	
	UPROPERTY()
	TSubclassOf<UGameplayAbility> ActiveItemAbility;
};
