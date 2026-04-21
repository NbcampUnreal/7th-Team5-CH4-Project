#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameModeBase.h"
#include "DDGameModeBase.generated.h"

class APlayerController;
class UAbilitySystemComponent;
class ADDBasePlayerController; 
class UGameplayEffect;
class UDDUIConfig; 
	
UCLASS()
class ADDGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADDGameModeBase();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void HandleSeamlessTravelPlayer(AController*& ParticipantController) override;
	virtual void GenericPlayerInitialization(AController* C) override;

	/** 상태 전환 가상 함수 (자식 클래스에서 오버라이드하여 사용) */
	virtual void SetMatchState(FGameplayTag NewStateTag);

	virtual void OnCharacterKilled(AActor* Killer, AActor* Victim);
	virtual void HandleRespawn(AController* TargetController);
	
	virtual void FocusAllCamerasOnTarget(AActor* TargetActor);

protected:
	/** 플레이어의 AbilitySystemComponent를 가져오는 공통 헬퍼 함수 */
	UAbilitySystemComponent* GetAbilitySystemComponentFromPlayer(APlayerController* PlayerController);
	virtual void SpawnSpectatorPawn(APlayerController* PlayerController);

	void BroadcastToAllPlayers(TFunctionRef<void(ADDBasePlayerController*)> Func);
	
	void BroadcastGameLayer(const FGameplayTag NewGameTag);
	
	void BroadcastErrorMessage(const FString& ErrorMessage);
	
	void BroadcastOpenPopUp(FGameplayTag Tag);
	
	void BroadcastClosePopUp(FGameplayTag Tag);
	
	void BroadcastCloseAllPopUps();
protected:
	/** 본 게임에 참여 중인 플레이어 컨트롤러 목록 (관전자 제외) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData", meta=(DisplayName="참여 중인 플레이어 컨트롤러 목록"))
	TArray<TObjectPtr<APlayerController>> AlivePlayerControllers;

	/** 공통 리스폰 시 부여할 이펙트들 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Response")
	TArray<TSubclassOf<UGameplayEffect>> ReSpawnEffectClasses;
	
protected:
	/**각 컨트롤러에 보여줄 UI 정보 */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TObjectPtr<UDDUIConfig> CurrentUIConfig;
};
