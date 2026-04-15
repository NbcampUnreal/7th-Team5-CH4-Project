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
	void BeginItemAction(FName ItemID, const FItemTableRow& ItemRow);
	
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

	/** 아이템 액션이 진행중인지 확인 : 타게팅할 때 + 범위표시할 때 아이템 액션 중인지 확인하고 수행 */
	UFUNCTION(BlueprintCallable, Category = "ItemActionComp")
	bool IsItemActionActive() const { return CurrentActionMode != EItemActionMode::None; }
	
public:
	/** 타겟 아이템으로 카메라를 이동하는 서버 RPC */
	UFUNCTION(Server, Reliable)
	void Server_FocusItemTarget(AActor* TargetActor);

	/** 즉시사용 아이템 액션 서버 RPC */
	UFUNCTION(Server, Reliable)
	void Server_ConfirmInstantItem(FName ItemID);

	/** 타게팅 아이템 액션 서버 RPC */
	UFUNCTION(Server, Reliable)
	void Server_ConfirmTargetingItem(FName ItemID, AActor* TargetActor);

	/** 범위 아이템 액션 서버 RPC */
	UFUNCTION(Server, Reliable)
	void Server_ConfirmRangeItem(FName ItemID);

protected:
	/** 즉시사용 아이템 액션 */
	void StartInstantAction();
	
	/** 타게팅 아이템 액션 */
	void StartTargetingAction();
	
	/** 범위 아이템 액션 */
	void StartRangeAction();
	
protected:
	/** 타겟 후보 대상을 순회해서 지정해주는 헬퍼 */
	void BuildTargetCandidates();
	
	/** Index를 원형으로 순회할 수 있도록 하는 계산 헬퍼 */
	void ChangeTarget(int32 Offset);

	/** 선택된 타겟 Getter */
	AActor* GetSelectedTarget() const;
	
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
	FGameplayTag ActiveItemType;
	
	UPROPERTY()
	TSubclassOf<UGameplayAbility> ActiveItemAbility;
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> CandidateTargets;
	
	UPROPERTY()
	int32 SelectedTargetIndex = INDEX_NONE;
};
