#include "MiniGames/Shooter/Abilities/GA_Shot.h"

#include "MiniGames/Shooter/Character/DDShooterCharacter.h"

void UGA_Shot::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ActorInfo == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ADDShooterCharacter* ShooterCharacter = Cast<ADDShooterCharacter>(ActorInfo->AvatarActor.Get());
	if (ShooterCharacter == nullptr || !ShooterCharacter->CanFire())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ShooterCharacter->TryFire();
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
