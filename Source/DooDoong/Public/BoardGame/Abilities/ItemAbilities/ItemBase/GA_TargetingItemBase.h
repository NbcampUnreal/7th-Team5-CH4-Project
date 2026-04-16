#pragma once

#include "CoreMinimal.h"
#include "BoardGame/Abilities/ItemAbilities/ItemBase/GA_ItemBase.h"
#include "GA_TargetingItemBase.generated.h"

UCLASS(Abstract)
class DOODOONG_API UGA_TargetingItemBase : public UGA_ItemBase
{
	GENERATED_BODY()

public:
	UGA_TargetingItemBase();

	/** Target이 없으면 선택 모드, Target이 결정되면 실행 */
	virtual void ActivateAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	/** 선택 모드 종료 시 타겟 후보를 정리 + 카메라 포커스를 복구 */
	virtual void EndAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

protected:
	/** 다음 타겟 선택 이벤트 처리 */
	UFUNCTION()
	void OnTargetNext(FGameplayEventData Payload);

	/** 이전 타겟 선택 이벤트 처리 */
	UFUNCTION()
	void OnTargetPrevious(FGameplayEventData Payload);

	/** 현재 선택된 타겟으로 아이템 효과를 확정 */
	UFUNCTION()
	void OnTargetConfirm(FGameplayEventData Payload);

	/** 타게팅 Ability를 취소 종료 -> 아이템 수 복구는 ItemActionComponent 취소에서 함 */
	UFUNCTION()
	void OnTargetCancel(FGameplayEventData Payload);

protected:
	/** 실제 타게팅 아이템 효과 구현 */
	virtual bool ExecuteTargetingItem(AActor* TargetActor);

protected:
	/** 현재 월드에서 자신을 제외한 보드게임 캐릭터를 타게팅 후보로 수집 */
	void BuildTargetCandidates();

	/** 후보 목록을 원형으로 순회 */
	void ChangeTarget(int32 Offset);

	/** 선택된 후보에게 카메라 포커스를 이동 */
	void FocusSelectedTarget();

	/** 보드게임 카메라를 지정한 대상에게 포커스 */
	void FocusCameraOnTarget(AActor* TargetActor);

	/** 현재 선택된 타겟을 반환 */
	AActor* GetSelectedTarget() const;

protected:
	/** 타게팅 선택 후보 목록 */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> CandidateTargets;

	/** 현재 선택된 후보 인덱스 */
	UPROPERTY()
	int32 SelectedTargetIndex = INDEX_NONE;

	/** Target 없이 활성화되어 입력을 기다리는 상태인지 여부. true가 되면 효과적용 */
	UPROPERTY()
	bool bSelectingTarget = false;
};
