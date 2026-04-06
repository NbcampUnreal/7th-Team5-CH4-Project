#include "AbilitySystem/Abilities/GA_Attack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "System/DDGameplayTags.h"

UGA_Attack::UGA_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; 
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	
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
	HitActors.Empty();
	
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
	HitActors.Add(HitActor);
	
	LaunchTarget(HitActor, KnockBackStrength);
	
	ApplyEffectsToTarget(HitActor);
}

void UGA_Attack::ApplyEffectsToTarget(AActor* TargetActor)
{
	IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(TargetActor);
	if (!TargetASI) return;
	
	UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent();
	if (!TargetASC) return;
	
	if (DamageEffectClass)
	{
		FGameplayEffectContextHandle Context = CachedAbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(CachedCharacter);
		
		FGameplayEffectSpecHandle SpecHandle =
			CachedAbilitySystemComponent->MakeOutgoingSpec(DamageEffectClass, 1.f, Context); 
		
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(DDGameplayTags::Data_Health_Damage, -DamageAmount);
			
			CachedAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		}
	}
	
	for (auto EffectClass : AdditionalEffectClasses)
	{
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

