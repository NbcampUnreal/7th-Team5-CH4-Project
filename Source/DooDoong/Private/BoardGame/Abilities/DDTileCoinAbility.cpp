#include "BoardGame/Abilities/DDTileCoinAbility.h"

#include "AbilitySystem/Attributes/DDPointSet.h"
#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/DDTile.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "Common/DDLogManager.h"

UDDTileCoinAbility::UDDTileCoinAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UDDTileCoinAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!HasAuthority(&ActivationInfo))
	{
		return; // 서버 실행
	}
	LOG_CYS(Warning, TEXT("Coin Tile Event!"));

	// 플레이어 스테이트 
	ADDBasePlayerState* PS = Cast<ADDBasePlayerState>(ActorInfo->OwnerActor.Get());
	if (!PS) return;
	
	ADDTile* Tile = PS->CurrentTile;
	if (!Tile) return;

	// 타일 정보 가져옴 
	int32 AddCoin = Tile->TileData.Effect;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return;

	// Spec 생성
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(GE_Coin, 1.f);
	if (!SpecHandle.IsValid()) return;

	// SetByCaller
	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag("Data.Point.Coin"),
		AddCoin
	);
	
	// 적용
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	
	LOG_CYS(Warning, TEXT("Coin + %d, Total %.0f"), AddCoin, ASC->GetNumericAttribute(UDDPointSet::GetCoinAttribute()));
	
	// UX
	ADDBoardGameCharacter* Character = Cast<ADDBoardGameCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return;
	
	Character->Multicast_ShowTileContentAboveHead(TileEventTag);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
