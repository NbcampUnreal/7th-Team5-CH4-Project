#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "DDBasePlayerState.generated.h"

class UDDPointSet;
class UDDAbilitySystemComponent;
class UDDHealthSet;
class UDDMovementSet;
class ADDTile; 
class ADDTileManager;
DECLARE_MULTICAST_DELEGATE(FOnGASReady);

USTRUCT(BlueprintType)
struct FPlayerGameplayInfo
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 SlotIndex = -1; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FLinearColor PlayerColor = FLinearColor::White; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName PlayerDisplayName = FName("");
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bPlayerIsDead = false; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    int32 TurnOrder = -1;
};

UCLASS()
class DOODOONG_API ADDBasePlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ADDBasePlayerState();

    FOnGASReady OnGASReady;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    FORCEINLINE UDDHealthSet* GetHealthSet() const { return HealthSet; }
    FORCEINLINE UDDPointSet* GetPointSet() const { return PointSet; }
    FORCEINLINE UDDMovementSet* GetMovementSet() const { return MovementSet; }

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
	virtual void BeginPlay() override;
public:
	UFUNCTION(NetMulticast, Reliable)
	void MultiCast_BroadcastKillLog(AActor* Victim, AActor* Killer);
	
public:
    /** 심리스 트래블 시 새 맵의 PlayerState로 데이터를 인수인계 */
    virtual void CopyProperties(APlayerState* PlayerState) override;
	
	UFUNCTION()
	void InitTile();
	
private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = true))
    TObjectPtr<UDDAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<UDDHealthSet> HealthSet;

    UPROPERTY()
    TObjectPtr<UDDPointSet> PointSet;

    UPROPERTY()
    TObjectPtr<UDDMovementSet> MovementSet;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ADDTile* CurrentTile;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName StartTileName;

    /** 선착순 4인에 포함된 게임 참여자(true)인지 관전자(false)인지 식별 */
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Player Info")
    bool bIsParticipant = false;
	
	/* 플레이어 정보 구조체 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Player Info")
	FPlayerGameplayInfo PlayerGameData;
};
