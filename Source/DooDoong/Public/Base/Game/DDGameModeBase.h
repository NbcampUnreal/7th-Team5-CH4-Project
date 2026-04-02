#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DDGameModeBase.generated.h"

class APlayerController;

UCLASS()
class ADDGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	// 심리스 트래블 플레이어 검증 전용 함수
	virtual void HandleSeamlessTravelPlayer(AController*& ParticipantController) override;

private:
	UFUNCTION()
	void OnMainTimerElapsed();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<APlayerController>> AlivePlayerControllers;

	FTimerHandle MainTimerHandle;
};
