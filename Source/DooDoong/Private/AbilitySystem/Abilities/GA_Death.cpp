#include "AbilitySystem/Abilities/GA_Death.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Base/Character/DDBaseCharacter.h"
#include "Base/Game/DDGameModeBase.h"
#include "Common/DDLogManager.h"
#include "System/DDGameplayTags.h"

UGA_Death::UGA_Death()
{
	bRetriggerInstancedAbility = false; 
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly; 
	
	ActivationBlockedTags.AddTag(DDGameplayTags::State_Character_Death); 
	ActivationOwnedTags.AddTag(DDGameplayTags::State_Character_Death);
	
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
	LOG_CYS(Warning, TEXT("UGA_Death : ActivateAbility Started 실행!!!!!"));
	ADDBaseCharacter* Character = Cast<ADDBaseCharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return; 
	
	// 2. 실행 중인 어빌리티 취소 
	GetAbilitySystemComponentFromActorInfo()->CancelAbilities(nullptr, nullptr, this); 

	// 3. 몽타주 재생 Task 
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
		if (bIsBoardGame) // 보드게임에서 로직 생략
		{
			RequestRespawn();
			return;
		}
		// 1. 래그돌 
		Character->MultiCast_HandleRagDoll(); 
		
		// 부활 타이머 시작 
		SetResponseTimer(RespawnDelay);
	}
}

void UGA_Death::SetResponseTimer(float InResponseTime)
{
	LOG_KMS(Warning, TEXT("UGA_Death : SetResponseTimer Started."));
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
	ADDBaseCharacter* Character = Cast<ADDBaseCharacter>(GetAvatarActorFromActorInfo());
	if (!Character || !Character->HasAuthority()) return;
	
	AController* Controller = Character->GetController();
	
	if (ADDGameModeBase* GM = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		LOG_KMS(Warning, TEXT("GA_Death : Success Request Respawn"))
		GM->HandleRespawn(Controller);
		
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false); 
}
