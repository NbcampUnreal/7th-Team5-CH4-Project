#include "Animation/Notify/AN_SendGameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAN_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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
