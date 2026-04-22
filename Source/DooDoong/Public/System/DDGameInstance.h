#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "DDGameInstance.generated.h"

class APlayerController;
class UGameplayAbility;
class UDataTable;
class UDDUIConfig;

UCLASS()
class DOODOONG_API UDDGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;
	
	// ==========================================
	// 글로벌 장기 데이터 (맵 전환 시에도 유지됨)
	// ==========================================

	/** 주현 : DDGameInstance Getter */
	static UDDGameInstance* Get(const UObject* WorldContext);
	UDDUIConfig* GetOrLoadUIConfig(const FSoftObjectPath& UIConfigPath);

	UFUNCTION(BlueprintCallable, Category = "Network|Dedicated Server")
	void ConnectToDedicatedServer(APlayerController* PlayerController) const;

	UFUNCTION(BlueprintCallable, Category = "Network|Steam")
	bool ShowSteamInviteOverlay(APlayerController* PlayerController) const;

	UFUNCTION(BlueprintPure, Category = "Network|Dedicated Server")
	FString GetDedicatedServerTravelURL() const;

	UFUNCTION(BlueprintCallable, Category = "Network|Dedicated Server")
	void SetDedicatedServerEndpoint(const FString& InAddress, int32 InPort);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Global Data", meta=(DisplayName = "현재 보드판 라운드 진행도"))
	int32 CurrentRound = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<UDataTable> SoundDataTable;
	
	/* 민재 : ItemDataTable */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UDataTable> ItemDataTable;
	
	
	// 로비에 있던 총 인원 수
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
    int32 ExpectedPlayerCount = 0;

private:
	void TryCreateDedicatedServerSession();
	void OnCreateDedicatedServerSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnSessionInviteAccepted(bool bWasSuccessful, int32 ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult);

	UPROPERTY()
	TMap<FString, TObjectPtr<UDDUIConfig>> CachedUIConfigs;
	
	UPROPERTY(EditDefaultsOnly, Category = "Preload")
	TArray<TSubclassOf<UGameplayAbility>> PreloadedAbilityClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Network|Dedicated Server")
	FString DedicatedServerAddress = TEXT("127.0.0.1");

	UPROPERTY(EditDefaultsOnly, Category = "Network|Dedicated Server")
	int32 DedicatedServerPort = 7777;

	UPROPERTY(EditDefaultsOnly, Category = "Network|Dedicated Server")
	int32 DedicatedServerMaxPlayers = 4;

	FDelegateHandle OnCreateSessionCompleteHandle;
	FDelegateHandle OnSessionInviteAcceptedHandle;
};
