#include "BoardGame/Abilities/DDTileBaseAbility.h"
#include "AbilitySystemComponent.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"

void UDDTileBaseAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ExecuteItemCue();
}

void UDDTileBaseAbility::ExecuteItemCue() const
{
	if (!CueTag.IsValid())
	{
		return;
	}

	const ADDBoardGameCharacter* BoardGameCharacter = Cast<ADDBoardGameCharacter>(GetAvatarActorFromActorInfo());
	if (!BoardGameCharacter) return;

	UAbilitySystemComponent* AbilitySystemComponent = BoardGameCharacter->GetAbilitySystemComponent();
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AbilitySystemComponent || !AvatarActor)
	{
		return;
	}

	FGameplayCueParameters CueParameters;
	CueParameters.Instigator = AvatarActor;
	CueParameters.EffectCauser = AvatarActor;
	CueParameters.Location = AvatarActor->GetActorLocation();

	AbilitySystemComponent->ExecuteGameplayCue(CueTag, CueParameters);
}
