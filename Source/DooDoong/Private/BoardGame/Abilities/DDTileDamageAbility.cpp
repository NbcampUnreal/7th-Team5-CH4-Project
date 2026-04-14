// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardGame/Abilities/DDTileDamageAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/DDHealthSet.h"
#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/DDTile.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "Common/DDLogManager.h"

UDDTileDamageAbility::UDDTileDamageAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UDDTileDamageAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!HasAuthority(&ActivationInfo))
	{
		return; // 서버 실행
	}
	LOG_CYS(Warning, TEXT("Damage Tile Event!"));

	// 플레이어 스테이트 
	ADDBasePlayerState* PS = Cast<ADDBasePlayerState>(ActorInfo->OwnerActor.Get());
	if (!PS) return;

	ADDTile* Tile = PS->CurrentTile;
	if (!Tile) return;

	// 타일 정보 가져옴 
	int32 Damage = -(Tile->TileData.Effect);

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return;

	// Spec 생성
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(GE_Damage, 1.f);
	if (!SpecHandle.IsValid()) return;

	// SetByCaller
	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag("Data.Health.Damage"),
		Damage
	);

	// 적용
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	LOG_CYS(Warning, TEXT("Damage %d, Total %.0f"), Damage,
	        ASC->GetNumericAttribute(UDDHealthSet::GetHealthAttribute()));

	// UX
	ADDBoardGameCharacter* Character = Cast<ADDBoardGameCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return;

	Character->Multicast_ShowTileContentAboveHead(TileEventTag);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
