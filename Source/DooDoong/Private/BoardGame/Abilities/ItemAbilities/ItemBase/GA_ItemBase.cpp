#include "BoardGame/Abilities/ItemAbilities/ItemBase/GA_ItemBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "Data/DDItemDataTypes.h"
#include "Data/ItemPayloadObject.h"
#include "System/DDGameplayTags.h"

UGA_ItemBase::UGA_ItemBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = DDGameplayTags::Event_Item_Activate;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	ActivationBlockedTags.AddTag(DDGameplayTags::State_Character_Death);
}

void UGA_ItemBase::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 손에 아이템 액터 붙이기
	if (HasAuthority(&ActivationInfo))
	{
		const UItemPayloadObject* PayloadObj = nullptr;

		if (TriggerEventData && TriggerEventData->OptionalObject)
		{
			PayloadObj = Cast<UItemPayloadObject>(TriggerEventData->OptionalObject);
		}

		if (PayloadObj)
		{
			CachedItemData = PayloadObj->ItemRow; // ← 반드시 캐싱
		}
	
		ItemActor = GetWorld()->SpawnActor<AActor>(CachedItemData.ItemActorClass);

		// 클라 복제
		ItemActor->SetReplicates(true);
		
		ItemActor->AttachToComponent(
			GetBoardGameCharacter()->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("RightHand")
		);
	}
}

void UGA_ItemBase::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 액터 파괴
	if (HasAuthority(&CurrentActivationInfo))
	{
		if (IsValid(ItemActor))
		{
			ItemActor->Destroy();
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

ADDBoardGameCharacter* UGA_ItemBase::GetBoardGameCharacter() const
{
	return Cast<ADDBoardGameCharacter>(GetAvatarActorFromActorInfo());
}

UAbilitySystemComponent* UGA_ItemBase::GetBoardGameAbilitySystemComponent() const
{
	const ADDBoardGameCharacter* BoardGameCharacter = GetBoardGameCharacter();
	return BoardGameCharacter ? BoardGameCharacter->GetAbilitySystemComponent() : nullptr;
}

bool UGA_ItemBase::ExecuteItemCue(const FGameplayTag& CueTag) const
{
	if (!CueTag.IsValid())
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystemComponent = GetBoardGameAbilitySystemComponent();
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AbilitySystemComponent || !AvatarActor)
	{
		return false;
	}

	FGameplayCueParameters CueParameters;
	CueParameters.Instigator = AvatarActor;
	CueParameters.EffectCauser = AvatarActor;
	CueParameters.Location = AvatarActor->GetActorLocation();

	AbilitySystemComponent->ExecuteGameplayCue(CueTag, CueParameters);
	return true;
}

bool UGA_ItemBase::ExecuteItemCueOnTarget(const FGameplayTag& CueTag, AActor* CueActor) const
{
	if (!CueTag.IsValid() || !IsValid(CueActor))
	{
		return false;
	}

	UAbilitySystemComponent* TargetAbilitySystemComponent =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CueActor);
	if (!TargetAbilitySystemComponent)
	{
		return false;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	FGameplayCueParameters CueParameters;
	CueParameters.Instigator = CueActor;
	CueParameters.EffectCauser = AvatarActor ? AvatarActor : CueActor;
	CueParameters.Location = CueActor->GetActorLocation();

	TargetAbilitySystemComponent->ExecuteGameplayCue(CueTag, CueParameters);
	return true;
}
