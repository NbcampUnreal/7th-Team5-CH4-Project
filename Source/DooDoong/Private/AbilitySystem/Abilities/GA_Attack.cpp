#include "AbilitySystem/Abilities/GA_Attack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "AbilitySystem/Attributes/DDHealthSet.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "System/DDGameplayTags.h"

UGA_Attack::UGA_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	AbilityTags.AddTag(DDGameplayTags::Input_Ability_Attack);
	ActivationOwnedTags.AddTag(DDGameplayTags::State_Character_Attacking);
	ActivationBlockedTags.AddTag(DDGameplayTags::State_Character_Death); 
}

void UGA_Attack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 1. Cache Owner Info 
	CachedCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	CachedAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CachedCharacter);

	// 2. Trace Task 
	UAbilityTask_WaitGameplayTagAdded* WaitTraceStartTask =
		UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, DDGameplayTags::Event_Trace_Start);

	WaitTraceStartTask->Added.AddDynamic(this, &ThisClass::OnReceiveTraceStart);
	WaitTraceStartTask->ReadyForActivation();

	UAbilityTask_WaitGameplayTagRemoved* WaitTraceEndTask =
		UAbilityTask_WaitGameplayTagRemoved::WaitGameplayTagRemove(this, DDGameplayTags::Event_Trace_Start);

	WaitTraceEndTask->Removed.AddDynamic(this, &ThisClass::OnReceiveTraceEnd);
	WaitTraceEndTask->ReadyForActivation();

	// 3. Montage Task 
	if (!AttackMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);

	PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCompleted);
	PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCompleted);

	PlayMontageTask->ReadyForActivation();
}

void UGA_Attack::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Attack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Attack::OnReceiveTraceStart()
{
	// 1. 중복 초기화 
	HitActors.Empty();

	// 2. 트레이스 타이머 시작 
	GetWorld()->GetTimerManager().SetTimer(
		TraceTimerHandle,
		this,
		&ThisClass::PerformTrace,
		0.1f,
		true
	);
}

void UGA_Attack::OnReceiveTraceEnd()
{
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
}

void UGA_Attack::PerformTrace()
{
	if (!CachedCharacter) return;

	// 1. Trace 
	FVector TraceLocation = CachedCharacter->GetMesh()->GetBoneLocation(TraceStartBone);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(CachedCharacter);
	FHitResult HitResult;

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		TraceLocation,
		TraceLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(TraceRadius),
		QueryParams
	);

	// 2. Debug 
	if (bShowDebugTrace)
	{
		DrawDebugSphere(
			GetWorld(),
			TraceLocation,
			TraceRadius,
			12,
			bHit ? HitColor : TraceColor,
			false,
			DebugDuration
		);
	}

	// 3. Apply Effects To Target 
	if (!bHit || HitActors.Contains(HitResult.GetActor())) return;
	AActor* HitActor = HitResult.GetActor();
	if (!HitActor) return;

	TryApplyDamageToActor(HitActor);
}

bool UGA_Attack::TryApplyDamageToActor(AActor* HitActor)
{
	if (!HasAuthority(&CurrentActivationInfo) || !HitActor) return false;

	// 1. 중복 처리 
	if (HitActors.Contains(HitActor)) return false;
	HitActors.Add(HitActor);

	// 2. 캐릭터 넉백 
	LaunchTarget(HitActor, KnockBackStrength);
	
	// 3. 데미지 적용 
	ApplyDamageToActor(HitActor);

	UE_LOG(LogTemp, Log, TEXT("[Attack] Hit:%s (Total:%d)"),
	       *HitActor->GetName(), HitActors.Num());
	
	return true;
}

void UGA_Attack::ApplyDamageToActor(AActor* TargetActor)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC || !CachedAbilitySystemComponent) return;

	// 1. 데미지 적용 
	if (DamageEffectClass)
	{
		FGameplayEffectContextHandle Context = CachedAbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(CachedCharacter);

		FGameplayEffectSpecHandle SpecHandle =
			CachedAbilitySystemComponent->MakeOutgoingSpec(DamageEffectClass, 1.f, Context);

		if (SpecHandle.IsValid())
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(DDGameplayTags::Data_Health_Damage, -DamageAmount);
			FActiveGameplayEffectHandle ActiveHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			UE_LOG(LogTemp, Log, TEXT("Applied %f Damage to %s"), DamageAmount, *TargetActor->GetName());
		}
	}

	// 2.추가 효과 적용 
	for (auto EffectClass : AdditionalEffectClasses)
	{
		if (!EffectClass) continue; 
		
		FGameplayEffectContextHandle Context = CachedAbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(CachedCharacter);

		FGameplayEffectSpecHandle Spechandle =
			CachedAbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.f, Context);
		if (Spechandle.IsValid())
		{
			CachedAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*Spechandle.Data.Get(), TargetASC);
		}
	}
}

void UGA_Attack::LaunchTarget(AActor* TargetActor, float Strength)
{
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	if (!TargetCharacter || !CachedCharacter) return;

	FVector LaunchDirection = TargetCharacter->GetActorLocation() - CachedCharacter->GetActorLocation();
	LaunchDirection.Z = 0.f;
	LaunchDirection.Normalize();

	FVector LaunchVelocity = LaunchDirection * Strength;
	LaunchVelocity.Z = 200.f;

	TargetCharacter->LaunchCharacter(LaunchVelocity, true, true);
}
