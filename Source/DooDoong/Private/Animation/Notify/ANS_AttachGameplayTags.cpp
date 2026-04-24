#include "Animation/Notify/ANS_AttachGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

void UANS_AttachGameplayTags::NotifyBegin(
	USkeletalMeshComponent* MeshComp, 
	UAnimSequenceBase* Animation,
	float TotalDuration, 
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(MeshComp->GetOwner());
	if (!ASI) return;
	
	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return;
	
	ASC->AddLooseGameplayTags(Tags);
}

void UANS_AttachGameplayTags::NotifyEnd(
	USkeletalMeshComponent* MeshComp, 
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(MeshComp->GetOwner());
	if (!ASI) return;
	
	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return;
	
	ASC->RemoveLooseGameplayTags(Tags); 
}
