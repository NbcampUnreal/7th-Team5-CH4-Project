#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/DDItemDataTypes.h"
#include "GameplayTagContainer.h"
#include "ItemActionComponent.generated.h"

UENUM(BlueprintType)
enum class EItemActionMode : uint8
{
	None,
	Instant,
	Targeting,
	Range
};

UENUM(BlueprintType)
enum class EItemTargetingInput : uint8
{
	Next,
	Previous,
	Confirm,
	Cancel
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DOODOONG_API UItemActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemActionComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	/** 아이템 사용 시작지점 */
	UFUNCTION(BlueprintCallable, Category = "ItemActionComp")
	void BeginItemAction(const FItemTableRow& ItemRow);
	
	/** 현재 아이템 액션을 확정 */
	UFUNCTION(BlueprintCallable, Category = "ItemActionComp")
	void ConfirmCurrentItemAction();
	
	/** 현재 아이템 액션을 취소하고 인벤토리 창을 다시 띄움 + 선차감된 아이템 수 복구 */
	UFUNCTION(BlueprintCallable, Category = "ItemActionComp")
	void CancelCurrentItemAction();

	/** 타게팅 Ability에 입력 이벤트 전달 */
	UFUNCTION(BlueprintCallable, Category = "ItemActionComp")
	void SendTargetingInput(EItemTargetingInput Input);

	/** 아이템 액션 중인지 확인 */
	UFUNCTION(BlueprintCallable, Category = "ItemActionComp")
	bool IsItemActionActive() const { return CurrentActionMode != EItemActionMode::None; }
	
public:
	/** 서버에서 이미 부여된 아이템 Ability를 이벤트로 실행 */
	UFUNCTION(Server, Reliable)
	void Server_ActivateItemAbility(FName ItemID, FGameplayTag ItemAbilityTag, AActor* TargetActor);

	/** 서버에서 진행 중인 타게팅 Ability에 입력 이벤트를 전달 */
	UFUNCTION(Server, Reliable)
	void Server_SendInputEvent(FGameplayTag EventTag);

protected:
	/** 서버에서 이미 부여된 아이템 Ability를 찾아 EventData를 보내면서 실행 */
	bool TryActivateItemAbility(FName ItemID, FGameplayTag ItemAbilityTag, AActor* TargetActor);

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
	UPROPERTY(Replicated)
	EItemActionMode CurrentActionMode = EItemActionMode::None;
	
	UPROPERTY(Replicated)
	FName ActiveItemID = NAME_None;
	
	UPROPERTY(Replicated)
	FGameplayTag ActiveItemTag;
	
	UPROPERTY(Replicated)
	FGameplayTag ActiveItemAbilityTag;
	
	FItemTableRow ItemRowData;
};
