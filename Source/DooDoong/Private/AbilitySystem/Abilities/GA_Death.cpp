#include "AbilitySystem/Abilities/GA_Death.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Base/Character/DDBaseCharacter.h"
#include "Base/Game/DDGameModeBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "System/DDGameplayTags.h"

UGA_Death::UGA_Death()
{
	bRetriggerInstancedAbility = false; 
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly; 
	
	ActivationBlockedTags.AddTag(DDGameplayTags::State_Character_Death); 
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = DDGameplayTags::Event_Character_Death;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_Death::ActivateAbility(
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
	
	ADDBaseCharacter* Character = Cast<ADDBaseCharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return; 
	
	GetAbilitySystemComponentFromActorInfo()->CancelAbilities(nullptr, nullptr, this); 
	
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(DDGameplayTags::State_Character_Death);
		
	UAbilityTask_PlayMontageAndWait* MontageTask = 
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, 
			TEXT("Death"),
			DeathMontage
		);
	
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_Death::OnMontageCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Death::OnMontageCompleted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Death::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Death::OnMontageCompleted);
	MontageTask->ReadyForActivation();
}

void UGA_Death::OnMontageCompleted()
{
	ADDBaseCharacter* Character = Cast<ADDBaseCharacter>(GetAvatarActorFromActorInfo());
	if(Character && Character->HasAuthority()) 
	{
		Character->MultiCast_HandleRagDoll(); 
		SetResponseTimer(RespawnDelay);
	}
}

void UGA_Death::SetResponseTimer(float InResponseTime)
{
	UE_LOG(LogTemp,Warning,TEXT("UGA_Death : SetResponseTimer Started."));
	GetWorld()->GetTimerManager().SetTimer(
		ResponseTimerHandle, 
		this, 
		&UGA_Death::RequestRespawn,
		InResponseTime,
		false
	);
}

void UGA_Death::RequestRespawn()
{
	UE_LOG(LogTemp,Warning,TEXT("GA_Death : RequestRespawn"))
	ADDBaseCharacter* Character = Cast<ADDBaseCharacter>(GetAvatarActorFromActorInfo());
	if (!Character || !Character->HasAuthority()) return;
	
	AController* Controller = Character->GetController();
	
	if (ADDGameModeBase* GM = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		UE_LOG(LogTemp,Warning,TEXT("GA_Death : Success RequestRespawn"))
		GM->HandleRespawn(Controller);
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false); 
}
