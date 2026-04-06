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

UCLASS()
class DOODOONG_API ADDBasePlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ADDBasePlayerState();

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    FORCEINLINE UDDHealthSet* GetHealthSet() const { return HealthSet; }
    FORCEINLINE UDDPointSet* GetPointSet() const { return PointSet; }
    FORCEINLINE UDDMovementSet* GetMovementSet() const { return MovementSet; }

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    /** 심리스 트래블 시 새 맵의 PlayerState로 데이터를 인수인계 */
    virtual void CopyProperties(APlayerState* PlayerState) override;

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
	
	UFUNCTION()
	void InitTile();
	
    /** 유저가 로비에서 설정한 닉네임 */
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Player Info")
    FString Nickname;

    /** 선착순 4인에 포함된 게임 참여자(true)인지 관전자(false)인지 식별 */
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Player Info")
    bool bIsParticipant = false;
};
