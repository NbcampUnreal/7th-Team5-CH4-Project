#include "BoardGame/Abilities/ItemAbilities/ItemBase/GA_RangeItemBase.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/Attributes/DDPointSet.h"
#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "Common/DDLogManager.h"
#include "System/DDGameplayTags.h"

UGA_RangeItemBase::UGA_RangeItemBase()
{
}

void UGA_RangeItemBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		LOG_CYS(Error, TEXT("RangeItem CommitAbility FAILED"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	LOG_CYS(Warning,TEXT("범위 아이템 어빌리티 실행"));
	TotalDrainedAmount = 0;
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		ADDBoardGameCharacter* MyChar = GetBoardGameCharacter();
		if (MyChar)
		{
			MyChar->bShowRangeIndicator = true;
			MyChar->OnRep_RangeIndicator(); // 서버에서도 즉시 반영
		}
	}
	
	// 타겟 업데이트
	GetWorld()->GetTimerManager().SetTimer(
		UpdateTimer,
		this,
		&UGA_RangeItemBase::UpdateDebug,
		0.1f,
		true
	);

	UAbilityTask_WaitGameplayEvent* ConfirmTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		DDGameplayTags::Event_Item_Target_Confirm
	);
	if (ConfirmTask)
	{
		ConfirmTask->EventReceived.AddDynamic(this, &ThisClass::OnConfirm);
		ConfirmTask->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* CancelTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		DDGameplayTags::Event_Item_Target_Cancel
	);
	if (CancelTask)
	{
		CancelTask->EventReceived.AddDynamic(this, &ThisClass::OnCancel);
		CancelTask->ReadyForActivation();
	}
}

void UGA_RangeItemBase::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(UpdateTimer);

	if (HasAuthority(&CurrentActivationInfo))
	{
		ADDBoardGameCharacter* MyChar =GetBoardGameCharacter();
		if (MyChar)
		{
			MyChar->bShowRangeIndicator = false;
			MyChar->OnRep_RangeIndicator();
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_RangeItemBase::UpdateDebug()
{
	ADDBoardGameCharacter* MyChar = Cast<ADDBoardGameCharacter>(GetAvatarActorFromActorInfo());
	if (!MyChar) return;

	FVector Origin = MyChar->GetActorLocation();
	FVector Forward = MyChar->GetActorForwardVector();

	DrawDebugCone(
		GetWorld(),
		Origin,
		Forward,
		Radius,
		FMath::DegreesToRadians(AngleDeg * 0.5f),
		FMath::DegreesToRadians(AngleDeg * 0.5f),
		20,
		FColor::Green,
		false,
		0.1f
	);
}

void UGA_RangeItemBase::FindTargets()
{
	ADDBoardGameCharacter* MyChar = Cast<ADDBoardGameCharacter>(GetAvatarActorFromActorInfo());
	if (!MyChar) return;

	FVector Origin = MyChar->GetActorLocation();
	FVector Forward = MyChar->GetActorForwardVector();

	CachedTargets.Empty();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APawn* Pawn = It->Get()->GetPawn();
		if (!Pawn) continue;

		ADDBoardGameCharacter* TargetChar = Cast<ADDBoardGameCharacter>(Pawn);
		if (!TargetChar || TargetChar == MyChar) continue;

		if (IsInFanRange(Origin, Forward, TargetChar->GetActorLocation()))
		{
			CachedTargets.Add(TargetChar);

			DrawDebugSphere(GetWorld(), TargetChar->GetActorLocation(), 30.f, 8, FColor::Red, false, 1.0f);
		}
	}
}

bool UGA_RangeItemBase::IsInFanRange(const FVector& Origin, const FVector& Forward, const FVector& TargetLoc)
{
	FVector ToTarget = TargetLoc - Origin;

	float Distance = ToTarget.Size();
	if (Distance > Radius) return false;

	ToTarget.Normalize();

	float Dot = FVector::DotProduct(Forward, ToTarget);
	float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));

	return Angle <= AngleDeg * 0.5f;
}

void UGA_RangeItemBase::OnConfirm(FGameplayEventData Payload)
{
	if (!HasAuthority(&CurrentActivationInfo))
	{
		return;
	}
	LOG_CYS(Warning,TEXT("범위 어빌리티 컨펌 입력 들어옴"));
	// 타겟 확정 시에만 탐색
	FindTargets();

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass, 1.f);
	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetSetByCallerMagnitude(EffectDataTag, EffectValue);

	// 타겟에게 GE 적용
	for (auto Target : CachedTargets)
	{
		ADDBasePlayerState* PS = Target->GetPlayerState<ADDBasePlayerState>();
		if (!PS) continue;

		UAbilitySystemComponent* TargetASC = PS->GetAbilitySystemComponent();
		if (!TargetASC) continue;

		// 적용
		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetASC);

		// 코인일 때만 저장
		if (EffectDataTag == FGameplayTag::RequestGameplayTag("Data.Point.Coin"))
		{
			const UDDPointSet* TargetAttributeSet = TargetASC->GetSet<UDDPointSet>();
			if (!TargetAttributeSet) continue;

			TotalDrainedAmount += TargetAttributeSet->LastCoinLose;
		}
	}
	LOG_CYS(Warning, TEXT("TotalDrainedAmount: %f"), TotalDrainedAmount);
}

void UGA_RangeItemBase::OnCancel(FGameplayEventData Payload)
{
	LOG_CYS(Warning,TEXT("범위 어빌리티 캔슬 입력 들어옴"));
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
