#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DDLobbyGameMode.generated.h"

class ADDLobbyPlayerController;

UCLASS()
class ADDLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADDLobbyGameMode();
	
	virtual void BeginPlay() override;
	
	virtual void Logout(AController* Exiting) override;
	
	/** 플레이어 등록 시도 (성공 시 true) */
	bool TryRegisterPlayerNickname(ADDLobbyPlayerController* Requester, const FName& Nickname, FString& ErrorMessage);
	
	/** 닉네임 중복 여부 확인 */
	bool IsNicknameAvailable(const FName& InNickname) const;
	
	void SetPlayerAsSpectator(APlayerController* InPlayerController);
	
protected:
	
	UFUNCTION()
	void OnMainTimerElapsed();

private:
	UPROPERTY()
	TArray<TObjectPtr<ADDLobbyPlayerController>> Participants;
	
	UPROPERTY()
	TArray<TObjectPtr<ADDLobbyPlayerController>> Spectators;

private:
	FTimerHandle MainTimerHandle;
	
	bool bIsStarting = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameSettings", meta=(DisplayName="N초 후 게임 시작"))
	int32 MaxWaitingTime = 5;
	
	int32 WaitingTime = MaxWaitingTime;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "GameSettings", meta=(DisplayName="보드게임 맵 경로"))
	FString BaseGameMapPath = TEXT("/Game/DooDoong/Map/L_BaseGame?listen");
	
protected:
	/** 남아있는 선택 가능한 색상 목록을 담아둘 배열 */
	UPROPERTY()
    TArray<FLinearColor> AvailableColors;
	
	/** 중복되지 않는 랜덤 색상을 반환하고 배열에서 제거하는 함수 */
	FLinearColor GetRandomAvailableColor();
};
