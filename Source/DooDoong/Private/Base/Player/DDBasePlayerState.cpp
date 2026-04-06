#include "Base/Player/DDBasePlayerState.h"
#include "AbilitySystem/DDAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/DDHealthSet.h"
#include "AbilitySystem/Attributes/DDPointSet.h"
#include "AbilitySystem/Attributes/DDMovementSet.h"
#include "Net/UnrealNetwork.h"
#include "Base/Game/DDGameStateBase.h"
#include "BoardGame/DDTileManager.h"
#include "Common/DDLogManager.h"

ADDBasePlayerState::ADDBasePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UDDAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);

	HealthSet = CreateDefaultSubobject<UDDHealthSet>(TEXT("HealthSet"));
	PointSet = CreateDefaultSubobject<UDDPointSet>(TEXT("PointSet"));
	MovementSet = CreateDefaultSubobject<UDDMovementSet>(TEXT("MovementSet"));

	SetNetUpdateFrequency(100.0f);
	
	StartTileName=TEXT("Tile01");
	
	// TODO:인스턴스에서 시작 타일 정보 백업 및 업뎃 필요
	
}

UAbilitySystemComponent* ADDBasePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADDBasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADDBasePlayerState, Nickname);
	DOREPLIFETIME(ADDBasePlayerState, bIsParticipant);
}

void ADDBasePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (ADDBasePlayerState* NewPlayerState = Cast<ADDBasePlayerState>(PlayerState))
	{
		// 커스텀 데이터 복제
		NewPlayerState->Nickname = this->Nickname;
		NewPlayerState->bIsParticipant = this->bIsParticipant;
	}
}

void ADDBasePlayerState::InitTile()
{
	LOG_CYS(Warning,TEXT("[PS]InitTile"));
	ADDGameStateBase* GameState = GetWorld()->GetGameState<ADDGameStateBase>();
	if (!GameState || !GameState->TileManager) return;

	CurrentTile = GameState->TileManager->FindTile(StartTileName);

}
