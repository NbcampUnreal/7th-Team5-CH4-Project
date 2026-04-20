#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "System/DDSoundManager.h"
#include "DDLobbyGameState.generated.h"

class ADDLobbyPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadyCountChanged, int32, ReadyCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerCountChanged, int32, PlayerCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountDownChanged, int32, CountDown);


UCLASS()
class DOODOONG_API ADDLobbyGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ADDLobbyGameState();
	
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_PlayerCount();
	
	UFUNCTION()
	void OnRep_ReadyCount();
	
	UFUNCTION()
	void OnRep_CountDown();
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopLobbyBGM();
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	int32 RequiredPlayerCount = 2; // 최소 게임 시작 인원 
	
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	int32 MaxPlayerCount = 4; // 최대 게임 인원 
	
	UPROPERTY(ReplicatedUsing = OnRep_ReadyCount)
	int32 ReadyCount = 0;
	
	UPROPERTY(ReplicatedUsing = OnRep_PlayerCount)
	int32 PlayerCount = 0;
	
	UPROPERTY(ReplicatedUsing = OnRep_CountDown)
	int32 CountDown = 0;
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerCountChanged OnPlayerCountChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnReadyCountChanged OnReadyCountChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnCountDownChanged OnCountDownChanged;
};


