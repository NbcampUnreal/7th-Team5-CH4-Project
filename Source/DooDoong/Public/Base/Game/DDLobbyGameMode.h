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
	int32 RemainWaitingTimeForStarting = 5;
	bool bIsStarting = false;

	// 에디터에서 설정할 수 있는 본 게임 맵 경로
	UPROPERTY(EditDefaultsOnly, Category = "Map Settings")
	FString BaseGameMapPath = TEXT("/Game/DooDoong/Map/L_BaseGame?listen");
};
