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
	
	// 게임 진행 중에는 false, 게임 종료 후 true
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    bool bIsGameFinished = false;
};

UCLASS()
class DOODOONG_API ADDBasePlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ADDBasePlayerState();
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
    virtual void CopyProperties(APlayerState* PlayerState) override;
	
public:
	// ==========================================
	// FORCEINLINE Getter / Setter
	// ==========================================
	
    FORCEINLINE UDDHealthSet* GetHealthSet() const { return HealthSet; }
    FORCEINLINE UDDPointSet* GetPointSet() const { return PointSet; }
    FORCEINLINE UDDMovementSet* GetMovementSet() const { return MovementSet; }
	
	FORCEINLINE int32 GetTurnOrder() const { return PlayerGameData.TurnOrder; }
	FORCEINLINE void SetTurnOrder(int32 InOrder) { PlayerGameData.TurnOrder = InOrder; }
	
	FORCEINLINE void SetIsGameFinished(bool bFinished) { PlayerGameData.bIsGameFinished = bFinished; }
	FORCEINLINE FName GetPlayerDisplayName() const { return PlayerGameData.PlayerDisplayName; }

public:
	UFUNCTION(NetMulticast, Reliable)
	void MultiCast_BroadcastKillLog(AActor* Victim, AActor* Killer);
	
	UFUNCTION()
	void InitTile();
	
	UFUNCTION(BlueprintCallable, Category = "PlayerState|Visuals")
    void SetPlayerColor(FLinearColor InNewColor);

	void UpdateCharacterVisuals();

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
	UPROPERTY(ReplicatedUsing = OnRep_PlayerGameData, VisibleAnywhere, BlueprintReadOnly, Category = "Player Info")
	FPlayerGameplayInfo PlayerGameData;
	
	/** 이 플레이어의 클라이언트 맵 로딩 및 스폰이 완전히 끝났는지 식별 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Info")
	bool bHasClientLoaded = false;
	
protected:
	UFUNCTION()
    void OnRep_PlayerGameData();
};
