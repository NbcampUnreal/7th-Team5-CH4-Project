#include "BoardGame/Abilities/DDTileGoalAbility.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/Attributes/DDPointSet.h"
#include "Base/Player/DDBasePlayerState.h"
#include "Common/DDLogManager.h"

UDDTileGoalAbility::UDDTileGoalAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UDDTileGoalAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	LOG_CYS(Warning, TEXT("Goal Tile Event!"));

	// 플레이어 스테이트 
	ADDBasePlayerState* PS = Cast<ADDBasePlayerState>(ActorInfo->OwnerActor.Get());
	if (!PS) return;

	if (HasAuthority(&ActivationInfo))
	{
		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
		if (!ASC) return;

		// Spec 생성
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(GE_Trophy, 1.f);
		if (!SpecHandle.IsValid()) return;

		// SetByCaller
		SpecHandle.Data->SetSetByCallerMagnitude(
			FGameplayTag::RequestGameplayTag("Data.Point.Trophy"),
			1
		);

		// 적용
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

		LOG_CYS(Warning, TEXT("Trophy + %.0f"), ASC->GetNumericAttribute(UDDPointSet::GetTrophyAttribute()));
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
