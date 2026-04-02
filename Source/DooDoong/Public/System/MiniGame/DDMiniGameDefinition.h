#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameTypes.h"
#include "Base/MiniGame/DDMiniGameRuleSet.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "Base/Player/DDBasePlayerController.h"
#include "Engine/DataAsset.h"
#include "Input/DDInputConfig.h"
#include "DDMiniGameDefinition.generated.h"

/**
 * 
 */
UCLASS()
class DOODOONG_API UDDMiniGameDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	/** 미니게임 ID */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	FName MiniGameID = NAME_None;

	/** 표시될 미니게임 이름 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	FText DisplayName;

	/** 표시될 미니게임 설명 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	FText Description;

	/** 미니게임 장르 태그 : 기본값 None */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame", meta=(Categories="MiniGame.Genre"))
	FGameplayTag Genre = DDGameplayTags::Genre_MiniGame_None;

	/** 미니게임 제한 시간 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	float DefaultTimeLimitSeconds = 60.f;

	/** 미니게임에 필요한 태그가 있으면 저장할 컨테이너 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	FGameplayTagContainer MiniGameTags;

	/** 필수 지정 : 미니게임 맵 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	TSoftObjectPtr<UWorld> MiniGameMap;

	/** 필수 지정 : 미니게임 모드 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	TSubclassOf<ADDMiniGameModeBase> GameModeClass;
	
	/** 필수 지정 : 미니게임 InputConfig */
	TSubclassOf<UDDInputConfig> InputConfigClass;
	
	/** 필수 지정 : 미니게임 IMC */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	TSubclassOf<UInputMappingContext> MappingContextClass;

	/** 선택 지정 : 따로 미니게임 RuleSet이 있다면 RuleSet */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	TSubclassOf<UDDMiniGameRuleSet> RuleSetClass;

};
