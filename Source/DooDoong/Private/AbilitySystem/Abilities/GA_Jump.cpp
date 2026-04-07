#include "AbilitySystem/Abilities/GA_Jump.h"
#include "GameFramework/Character.h"
#include "System/DDGameplayTags.h"

UGA_Jump::UGA_Jump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; 
	
	AbilityTags.AddTag(DDGameplayTags::Input_Ability_Jump);
	ActivationBlockedTags.AddTag(DDGameplayTags::State_Character_Death); 
}

bool UGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;  
	
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	return Character && Character->CanJump(); 
}

void UGA_Jump::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;
	
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return;
	
	Character->Jump();
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
