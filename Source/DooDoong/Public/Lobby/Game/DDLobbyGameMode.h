#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DDLobbyGameMode.generated.h"

class ADDLobbyGameState;
class UDDUIConfig;
class ADDLobbyPlayerController;

UCLASS()
class DOODOONG_API ADDLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADDLobbyGameMode();
	
	virtual void InitGameState() override;
	
	virtual void GenericPlayerInitialization(AController* C) override;
	
	virtual void Logout(AController* Exiting) override;
	
	/** 플레이어 등록 시도 (성공 시 true) */
	bool TryRegisterPlayerNickname(ADDLobbyPlayerController* Requester, const FName& Nickname, FString& ErrorMessage);
	
	/** 컨트롤러에서 레디 요청 */
	void RequestPlayerReady(ADDLobbyPlayerController* Requester, bool bReady);

protected:
	virtual void BeginPlay() override;
	
	ADDLobbyGameState* GetLobbyGameState();
	
	/** 닉네임 중복 여부 확인 */
	bool IsNicknameAvailable(const FName& InNickname, FString& ErrorMessage);
	
	/** 관전자로 전환*/
	void SetPlayerAsSpectator(APlayerController* InPlayerController);
	
	/** 게임 시작 카운트 다운 */
	void StartPlayCountDown(float InSeconds); 
	
	/** 카운트 다운 콜백 */
	UFUNCTION()
	void OnMainTimerElapsed();

	/** 보드게임 시작 여부 확인 */
	bool CanStartBoardGame(FString& ErrorMessage);
	
	/** 보드 게임 시작 */
	void TryStartBoardGame(); 

	/** 중복되지 않는 랜덤 색상을 반환하고 배열에서 제거하는 함수 */
	FLinearColor GetRandomAvailableColor();
	
private:
	UPROPERTY() /** 전체 참가자 */
	TArray<TObjectPtr<ADDLobbyPlayerController>> Participants;
	
	UPROPERTY() /** 준비 완료된 참가자 */
	TArray<TObjectPtr<ADDLobbyPlayerController>> ReadyParticipants;
	
	UPROPERTY() /** 관전자 */
	TArray<TObjectPtr<ADDLobbyPlayerController>> Spectators;

	UPROPERTY() /** 로비 게임 스테이트 */
	TObjectPtr<ADDLobbyGameState> CachedGameState = nullptr;
	
private:
	/** 카운트 다운 타이머 핸들*/
	FTimerHandle StartTimerHandle;
	
	/** 카운트 다운 시작 초 */
	UPROPERTY(EditDefaultsOnly, Category = "GameSettings", meta=(DisplayName="N초 후 게임 시작"))
	float StartCountdownSeconds = 5.f; 

private:
	UPROPERTY(EditDefaultsOnly, Category = "GameSettings", meta=(DisplayName="보드게임 맵 경로"))
	FString BoardGameMapPath = TEXT("/Game/DooDoong/Map/L_BoardMap?listen");
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "GameSettings|Color", meta=(DisplayName="기본 플레이어 색상 목록"))
    TArray<FLinearColor> DefaultPlayerColors;
	
	UPROPERTY(VisibleAnywhere, Transient, Category = "GameSettings|Color", meta=(DisplayName="현재 남은 색상 목록"))
    TArray<FLinearColor> AvailableColors = {
		FLinearColor(0.407f, 0.73f, 0.791f, 1.0f),
		FLinearColor(1.0f, 0.711f, 0.186f, 1.0f), 
		FLinearColor(0.597f, 0.402f, 0.597f, 1.0f),
		FLinearColor(1.0f, 0.305f, 0.254f, 1.0f),
	};

protected:
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	TObjectPtr<UDDUIConfig> LobbyUIConfig;
};
