#include "AbilitySystem/Abilities/GA_Jump.h"
#include "GameFramework/Character.h"
#include "System/DDGameplayTags.h"

UGA_Jump::UGA_Jump()
{
	SetAssetTags(FGameplayTagContainer(DDGameplayTags::Input_Ability_Jump));
}

void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;
	
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return;
	
	Character->Jump();
}
