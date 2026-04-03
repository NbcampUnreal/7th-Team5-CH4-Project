#include "Animation/Notify/AN_SendGameplayTag.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

void UAN_SendGameplayTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(MeshComp->GetOwner());
	if (!ASI) return; 
	
	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return;
	
	ASC->AddLooseGameplayTags(Tags); 
}
