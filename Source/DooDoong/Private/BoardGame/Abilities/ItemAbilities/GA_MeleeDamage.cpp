#include "BoardGame/Abilities/ItemAbilities/GA_MeleeDamage.h"

void UGA_MeleeDamage::OnConfirm(FGameplayEventData Payload)
{
	Super::OnConfirm(Payload);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
