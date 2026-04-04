#include "Animation/Notify/AN_SendGameplayTag.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

void UAN_SendGameplayTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	AActor* Owner = MeshComp->GetOwner();
	
	FGameplayEventData EventData;
	EventData.Instigator = Owner;
	EventData.Target = Owner;
	
	for (const FGameplayTag& Tag : Tags)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, Tag, EventData);
	}
}
