#pragma once

#include "CoreMinimal.h"
#include "DDLobbyGameState.h"
#include "GameFramework/GameModeBase.h"
#include "DDLobbyGameMode.generated.h"

class ADDLobbyGameState;
class UDDUIConfig;
class ADDLobbyPlayerController;

UENUM()
enum class ELobbyPlayerRole : uint8
{
	Participant,  // 참가자
	Spectator,    // 관전자
};

USTRUCT(BlueprintType)
struct FLobbyParticipantInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ADDLobbyPlayerController> Controller = nullptr;

	UPROPERTY()
	bool bIsReady = false;
	
	UPROPERTY()
	ELobbyPlayerRole Role = ELobbyPlayerRole::Participant;
};

UCLASS()
class DOODOONG_API ADDLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADDLobbyGameMode();
	
	virtual void InitGameState() override;
	
	virtual void Logout(AController* Exiting) override;
	
	virtual void GenericPlayerInitialization(AController* C) override;
	
	/** 플레이어 등록 시도 (성공 시 true) */
	bool TryRegisterPlayerNickname(ADDLobbyPlayerController* Requester, const FName& Nickname, FString& ErrorMessage);
	
	/** 컨트롤러에서 레디 요청 */
	void RequestPlayerReady(ADDLobbyPlayerController* Requester, bool bReady);

	void AssignPlayerRole(ADDLobbyPlayerController* Requester, ELobbyPlayerRole RequestedRole = ELobbyPlayerRole::Participant);
	
protected:
	virtual void BeginPlay() override;
	
	ADDLobbyGameState* GetLobbyGameState();
	
	/** 닉네임 중복 여부 확인 */
	bool IsNicknameAvailable(const FName& InNickname, FString& ErrorMessage);
	
	/** 관전자로 전환*/
	void SetPlayerAsSpectator(APlayerController* InPlayerController);
	
	/** 게임 시작 카운트 다운 */
	void StartBoardGameCountdown(float InSeconds); 
	
	/** 카운트 다운 콜백 */
	UFUNCTION()
	void OnCountdownTick();

	/** 보드게임 시작 여부 확인 */
	bool CanStartBoardGame(FString& ErrorMessage);
	
	/** 보드 게임 시작 */
	void TryStartBoardGame(); 

	/** 중복되지 않는 랜덤 색상을 반환하고 배열에서 제거하는 함수 */
	FLinearColor GetRandomAvailableColor();
	
	/** 현재 참가자 수 */
	int32 GetParticipantCount() const;
	
	/** 컨트롤러로 플레이어 정보가져오기 */
	FLobbyParticipantInfo* FindParticipant(ADDLobbyPlayerController* Requester);
	
private:
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	TObjectPtr<UDDUIConfig> LobbyUIConfig;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameSettings", meta=(DisplayName="보드게임 맵 경로"))
	FString BoardGameMapPath = TEXT("/Game/DooDoong/Map/L_BoardMap?listen");
	
	UPROPERTY()  // 로비에 있는 플레이어 정보들 (컨트롤러 / 준비 여부 / 관전자 여부 ) 
	TArray<FLobbyParticipantInfo> Participants; 
	
	UPROPERTY() /** 로비 게임 스테이트 */
	TObjectPtr<ADDLobbyGameState> CachedGameState = nullptr;
	
protected:
	/** 카운트 다운 타이머 핸들*/
	FTimerHandle StartTimerHandle;
	
	/** 카운트 다운 시작 초 */
	UPROPERTY(EditDefaultsOnly, Category = "GameSettings", meta=(DisplayName="N초 후 게임 시작"))
	float StartCountdownSeconds = 5.f; 
	
	UPROPERTY(EditDefaultsOnly, Category = "GameSettings|Color", meta=(DisplayName="기본 플레이어 색상 목록"))
    TArray<FLinearColor> DefaultPlayerColors;
	
	UPROPERTY(VisibleAnywhere, Transient, Category = "GameSettings|Color", meta=(DisplayName="현재 남은 색상 목록"))
    TArray<FLinearColor> AvailableColors = {
		FLinearColor(0.407f, 0.73f, 0.791f, 1.0f),
		FLinearColor(1.0f, 0.711f, 0.186f, 1.0f), 
		FLinearColor(0.597f, 0.402f, 0.597f, 1.0f),
		FLinearColor(1.0f, 0.305f, 0.254f, 1.0f),
	};


};
