#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DDGameStateBase.generated.h"

// 게임의 진행 상태를 관리하는 Enum 
UENUM(BlueprintType)
enum class EDDMatchState : uint8
{
	None,
	Waiting,
	Starting,
	Playing,
	Ending,
	End
};

class ADDTileManager;

UCLASS()
class ADDGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	EDDMatchState MatchState = EDDMatchState::Waiting;
	
	UPROPERTY()
	ADDTileManager* TileManager;
};
