#include "BoardGame/Abilities/ItemCueBase/DDItemGameplayCueNotify_Static.h"

#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "System/DDSoundManager.h"

bool UDDItemGameplayCueNotify_Static::OnExecute_Implementation(
	AActor* MyTarget,
	const FGameplayCueParameters& Parameters) const
{
	Super::OnExecute_Implementation(MyTarget, Parameters);

	PlayAnimation(Parameters);
	PlaySound(MyTarget, Parameters);
	SpawnEffect(MyTarget, Parameters);

	return true;
}

void UDDItemGameplayCueNotify_Static::PlayAnimation(const FGameplayCueParameters& Parameters) const
{
	if (!Montage)
	{
		return;
	}

	AActor* Instigator = Parameters.GetInstigator();
	if (!Instigator)
	{
		return;
	}

	USkeletalMeshComponent* MeshComponent = Instigator->FindComponentByClass<USkeletalMeshComponent>();
	if (!MeshComponent)
	{
		return;
	}

	UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	const float MontageLength = AnimInstance->Montage_Play(Montage, PlayRate);
	if (MontageLength <= 0.f)
	{
		return;
	}

	if (!MontageStartSection.IsNone())
	{
		AnimInstance->Montage_JumpToSection(MontageStartSection, Montage);
	}
}

void UDDItemGameplayCueNotify_Static::PlaySound(
	const UObject* WorldContextObject,
	const FGameplayCueParameters& Parameters) const
{
	if (SoundID.IsNone() || !WorldContextObject)
	{
		return;
	}

	UDDSoundManager* SoundManager = UDDSoundManager::Get(WorldContextObject);
	if (!SoundManager)
	{
		return;
	}

	SoundManager->PlaySoundAtLocation(SoundID, Parameters.Location);
}

void UDDItemGameplayCueNotify_Static::SpawnEffect(
	const UObject* WorldContextObject,
	const FGameplayCueParameters& Parameters) const
{
	if (!Effect || !WorldContextObject)
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		WorldContextObject,
		Effect,
		Parameters.Location,
		FRotator::ZeroRotator,
		EffectScale
	);
}
