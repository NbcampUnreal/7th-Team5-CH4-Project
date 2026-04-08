#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DDLobbyGameMode.generated.h"

UCLASS()
class ADDLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Logout(AController* Exiting) override;

	void ProcessPlayerJoin(class ADDLobbyPlayerController* LobbyPlayerController, const FString& Nickname);
	
protected:
	UFUNCTION()
	void OnMainTimerElapsed();

private:
	TArray<TObjectPtr<ADDLobbyPlayerController>> Participants;
	TArray<TObjectPtr<ADDLobbyPlayerController>> Spectators;

	FTimerHandle MainTimerHandle;
	
	bool bIsStarting = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameSettings", meta=(DisplayName="N초 후 게임 시작"))
	int32 MaxWaitingTime = 5;
	
	int32 WaitingTime = MaxWaitingTime;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameSettings", meta=(DisplayName="보드게임 맵 경로"))
	FString BaseGameMapPath = TEXT("/Game/DooDoong/Map/L_BaseGame?listen");
};
