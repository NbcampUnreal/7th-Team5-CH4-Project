#include "AbilitySystem/Abilities/GA_Attack.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Character.h"
#include "System/DDGameplayTags.h"

UGA_Attack::UGA_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; 
	SetAssetTags(FGameplayTagContainer(DDGameplayTags::Input_Ability_Attack)); 
}

bool UGA_Attack::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
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
	
	CachedCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	
	if (!AttackMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UAbilityTask_WaitGameplayEvent* WaitTraceStartTask = 
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DDGameplayTags::Event_Trace_Start); 
	
	WaitTraceStartTask->EventReceived.AddDynamic(this, &ThisClass::OnReceiveTraceStart);
	WaitTraceStartTask->ReadyForActivation(); 
	
	UAbilityTask_WaitGameplayEvent* WaitTraceEndTask = 
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DDGameplayTags::Event_Trace_End); 
	
	WaitTraceEndTask->EventReceived.AddDynamic(this, &ThisClass::OnReceiveTraceEnd);
	WaitTraceEndTask->ReadyForActivation(); 
	
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

void UGA_Attack::OnReceiveTraceStart(FGameplayEventData EventData)
{
	PerformTrace(); 
	
	GetWorld()->GetTimerManager().SetTimer(
		TraceTimerHandle,
		this,
		&ThisClass::PerformTrace,
		0.1f, 
		true
		); 
}

void UGA_Attack::OnReceiveTraceEnd(FGameplayEventData EventData)
{
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
}

void UGA_Attack::PerformTrace()
{
	if (!CachedCharacter) return; 
	
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
	
	if (!bHit) return;
	
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
	
	IAbilitySystemInterface* OwnerASI = Cast<IAbilitySystemInterface>(CachedCharacter); 
	IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(HitResult.GetActor());
	if (!OwnerASI || !TargetASI) return;
	
	UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent();
	UAbilitySystemComponent* OwnerASC = OwnerASI->GetAbilitySystemComponent();
	if (!OwnerASC || !TargetASC) return;
	
	if (DamageEffectClass)
	{
		FGameplayEffectContextHandle Context = OwnerASC->MakeEffectContext();
		Context.AddSourceObject(CachedCharacter);
		
		FGameplayEffectSpecHandle Spechandle =OwnerASC->MakeOutgoingSpec(DamageEffectClass, 1.f, Context); 
		if (Spechandle.IsValid())
		{
			OwnerASC->ApplyGameplayEffectSpecToTarget(*Spechandle.Data.Get(), TargetASC);
		}
	}
	
	for (auto EffectClass : AdditionalEffectClasses)
	{
		FGameplayEffectContextHandle Context = OwnerASC->MakeEffectContext();
		Context.AddSourceObject(CachedCharacter);
		
		FGameplayEffectSpecHandle Spechandle =OwnerASC->MakeOutgoingSpec(EffectClass, 1.f, Context); 
		if (Spechandle.IsValid())
		{
			OwnerASC->ApplyGameplayEffectSpecToTarget(*Spechandle.Data.Get(), TargetASC);
		}
	}
}
