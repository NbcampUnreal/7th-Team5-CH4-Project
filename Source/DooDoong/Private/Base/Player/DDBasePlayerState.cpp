#include "Base/Player/DDBasePlayerState.h"
#include "AbilitySystem/DDAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/DDHealthSet.h"
#include "AbilitySystem/Attributes/DDPointSet.h"
#include "AbilitySystem/Attributes/DDMovementSet.h"
#include "Base/Character/DDBaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Base/Game/DDGameStateBase.h"
#include "BoardGame/DDTileManager.h"
#include "Common/DDLogManager.h"
#include "UI/Inventory/DDInventoryComponent.h"

ADDBasePlayerState::ADDBasePlayerState()
{
	InventoryComponent = CreateDefaultSubobject<UDDInventoryComponent>(TEXT("InventoryComponent"));
	
	AbilitySystemComponent = CreateDefaultSubobject<UDDAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);

	HealthSet = CreateDefaultSubobject<UDDHealthSet>(TEXT("HealthSet"));
	PointSet = CreateDefaultSubobject<UDDPointSet>(TEXT("PointSet"));
	MovementSet = CreateDefaultSubobject<UDDMovementSet>(TEXT("MovementSet"));

	SetNetUpdateFrequency(100.0f);

	StartTileName = TEXT("Tile01");
}

UAbilitySystemComponent* ADDBasePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADDBasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADDBasePlayerState, PlayerGameData);
	DOREPLIFETIME(ADDBasePlayerState, bIsParticipant);
}

void ADDBasePlayerState::BeginPlay()
{
	Super::BeginPlay();

	HealthSet->KillLogSignature.AddUObject(this, &ThisClass::MultiCast_BroadcastKillLog);
}

void ADDBasePlayerState::MultiCast_BroadcastKillLog_Implementation(AActor* Victim, AActor* Killer)
{
	UE_LOG(LogTemp, Warning, TEXT("[PS] : %s was killed by %s"), *Victim->GetName(), *Killer->GetName());
}

void ADDBasePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (ADDBasePlayerState* NewPlayerState = Cast<ADDBasePlayerState>(PlayerState))
	{
		// ========================================================
		// 1. 공통 유지 데이터 (언제나 유지)
		// ========================================================
		NewPlayerState->PlayerGameData.PlayerDisplayName = this->PlayerGameData.PlayerDisplayName;
		NewPlayerState->PlayerGameData.PlayerColor = this->PlayerGameData.PlayerColor;

		NewPlayerState->bIsParticipant = this->bIsParticipant;
		NewPlayerState->PlayerGameData.bIsGameFinished = this->PlayerGameData.bIsGameFinished;
		NewPlayerState->InventoryComponent = this->InventoryComponent;


		// 미니게임과 보드게임을 오가는 중
		if (NewPlayerState->PlayerGameData.bIsGameFinished == false)
		{
			NewPlayerState->PlayerGameData.TurnOrder = this->PlayerGameData.TurnOrder;
			NewPlayerState->StartTileName = this->StartTileName;

			// AtrributeSet 유지
			if (this->PointSet && NewPlayerState->PointSet)
			{
				NewPlayerState->PointSet->SetTrophy(this->PointSet->GetTrophy());
				NewPlayerState->PointSet->SetCoin(this->PointSet->GetCoin());
			}
			if (this->HealthSet && NewPlayerState->HealthSet)
			{
				NewPlayerState->HealthSet->SetHealth(this->HealthSet->GetHealth());
				NewPlayerState->HealthSet->SetMaxHealth(this->HealthSet->GetMaxHealth());
			}
			if (this->MovementSet && NewPlayerState->MovementSet)
			{
				NewPlayerState->MovementSet->SetMoveSpeed(this->MovementSet->GetMoveSpeed());
				NewPlayerState->MovementSet->SetJumpSpeed(this->MovementSet->GetJumpSpeed());
			}
		}
		// 게임이 끝나고 로비로 복귀함
		else
		{
			NewPlayerState->PlayerGameData.bIsGameFinished = false;
			NewPlayerState->PlayerGameData.TurnOrder = -1;

			// AtrributeSet 초기값으로 초기화
			if (NewPlayerState->PointSet)
			{
				NewPlayerState->PointSet->SetTrophy(0.0f);
				NewPlayerState->PointSet->SetCoin(0.0f);
			}
			if (this->HealthSet && NewPlayerState->HealthSet)
			{
				NewPlayerState->HealthSet->SetHealth(this->HealthSet->GetMaxHealth());
				NewPlayerState->HealthSet->SetMaxHealth(this->HealthSet->GetMaxHealth());
			}
		}
	}
}

void ADDBasePlayerState::SetCurrentRank(int32 InRank)
{
	if (PlayerGameData.CurrentRank != InRank)
    {
		FPlayerGameplayInfo TempData = PlayerGameData;
        TempData.CurrentRank = InRank;
		
        PlayerGameData = TempData;
		
        OnRankChanged.Broadcast(InRank);
    }
}

void ADDBasePlayerState::InitTile()
{
	LOG_CYS(Warning, TEXT("[PS]InitTile"));
	ADDGameStateBase* GameState = GetWorld()->GetGameState<ADDGameStateBase>();
	if (!GameState || !GameState->TileManager) return;

	CurrentTile = GameState->TileManager->FindTile(StartTileName);
}

void ADDBasePlayerState::OnRep_PlayerGameData()
{
	UpdateCharacterVisuals();
	
	OnRankChanged.Broadcast(PlayerGameData.CurrentRank);
}

void ADDBasePlayerState::SetPlayerColor(FLinearColor InNewColor)
{
	if (HasAuthority())
	{
		PlayerGameData.PlayerColor = InNewColor;

		UpdateCharacterVisuals();
	}
}

void ADDBasePlayerState::UpdateCharacterVisuals()
{
if (ADDBaseCharacter* BaseCharacter = Cast<ADDBaseCharacter>(GetPawn()))
	{
		BaseCharacter->ApplyColorFromPlayerState(PlayerGameData.PlayerColor);
	}
}

