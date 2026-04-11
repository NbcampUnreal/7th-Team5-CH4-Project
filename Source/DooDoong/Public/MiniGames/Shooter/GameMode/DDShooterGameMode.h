#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "DDShooterGameMode.generated.h"

class ADDShooterCharacter;

UCLASS()
class DOODOONG_API ADDShooterGameMode : public ADDMiniGameModeBase
{
	GENERATED_BODY()

public:
	ADDShooterGameMode();

	/** 서버에서만 호출 
	 * ShooterCharacter가 계산한 발사 위치/회전을 기준으로 발사체를 스폰한다. */
	void FireProjectile(ADDShooterCharacter* ShooterCharacter, const FVector& SpawnLocation, const FRotator& SpawnRotation);
	
	/** 서버에서만 호출
	 * 타겟 액터를 스폰 */
	void SpawnTarget();
};
