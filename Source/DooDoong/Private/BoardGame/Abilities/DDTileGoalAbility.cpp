#include "BoardGame/Abilities/DDTileGoalAbility.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/Attributes/DDPointSet.h"
#include "Base/Player/DDBasePlayerState.h"
#include "Common/DDLogManager.h"

UDDTileGoalAbility::UDDTileGoalAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
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
	
	// Montage
	if (!MontageToPlay)
	{
		LOG_CYS(Error, TEXT("MontageToPlay 없음"));
		return;
	}
	if (!ActorInfo->GetAnimInstance())
	{
		LOG_CYS(Error, TEXT("AnimInstance 없음"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* Task =
	UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		MontageToPlay
	);

	Task->OnCompleted.AddDynamic(this, &UDDTileGoalAbility::OnMontageCompleted);
	Task->OnInterrupted.AddDynamic(this, &UDDTileGoalAbility::OnMontageInterrupted);
	Task->ReadyForActivation();
	
}
void UDDTileGoalAbility::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UDDTileGoalAbility::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}