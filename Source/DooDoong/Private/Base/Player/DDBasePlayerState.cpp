#include "Base/Player/DDBasePlayerState.h"
#include "AbilitySystem/DDAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/DDHealthSet.h"
#include "AbilitySystem/Attributes/DDPointSet.h"
#include "AbilitySystem/Attributes/DDMovementSet.h"
#include "Net/UnrealNetwork.h"
#include "Base/Game/DDGameStateBase.h"
#include "BoardGame/DDTile.h"
#include "Base/MiniGame/DDMiniGameStateBase.h"
#include "BoardGame/DDTileManager.h"
#include "Common/DDLogManager.h"
#include "GameFramework/Character.h"

ADDBasePlayerState::ADDBasePlayerState()
{
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
       // 1. 일반 변수 및 구조체 복사
       // ========================================================
       NewPlayerState->PlayerGameData.PlayerDisplayName = this->PlayerGameData.PlayerDisplayName;
       NewPlayerState->PlayerGameData.PlayerColor = this->PlayerGameData.PlayerColor;
       NewPlayerState->PlayerGameData.TurnOrder = this->PlayerGameData.TurnOrder;
       NewPlayerState->PlayerGameData.SlotIndex = this->PlayerGameData.SlotIndex;
       NewPlayerState->PlayerGameData.bPlayerIsDead = this->PlayerGameData.bPlayerIsDead;
       
       NewPlayerState->bIsParticipant = this->bIsParticipant;
       NewPlayerState->StartTileName = this->StartTileName; 
       
       // ========================================================
       // 2. GAS AttributeSet 깊은 복사
       // ========================================================
       
       // [DDHealthSet 복사]
       if (this->HealthSet && NewPlayerState->HealthSet)
       {
           NewPlayerState->HealthSet->SetHealth(this->HealthSet->GetHealth());
           NewPlayerState->HealthSet->SetMaxHealth(this->HealthSet->GetMaxHealth());
       }

       // [DDPointSet 복사]
       if (this->PointSet && NewPlayerState->PointSet)
       {
           NewPlayerState->PointSet->SetTrophy(this->PointSet->GetTrophy());
           NewPlayerState->PointSet->SetCoin(this->PointSet->GetCoin());
       }

       // [DDMovementSet 복사]
       if (this->MovementSet && NewPlayerState->MovementSet)
       {
           NewPlayerState->MovementSet->SetMoveSpeed(this->MovementSet->GetMoveSpeed());
           NewPlayerState->MovementSet->SetJumpSpeed(this->MovementSet->GetJumpSpeed());
       }
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
	APawn* OwningPawn = GetPawn();
	if (!OwningPawn) return;

	ACharacter* Character = Cast<ACharacter>(OwningPawn);
	if (Character && Character->GetMesh())
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();

		if (!CachedColorMaterial || Mesh->GetMaterial(0) != CachedColorMaterial)
        {
            CachedColorMaterial = Mesh->CreateDynamicMaterialInstance(0);
        }
		
		if (CachedColorMaterial)
		{
			CachedColorMaterial->SetVectorParameterValue(TEXT("PlayerColor"), PlayerGameData.PlayerColor);
		}
	}
}
