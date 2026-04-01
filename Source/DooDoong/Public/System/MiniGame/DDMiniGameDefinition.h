#pragma once

#include "CoreMinimal.h"
#include "Base/MiniGame/DDMiniGameTypes.h"
#include "Base/MiniGame/DDMiniGameRuleSet.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "Engine/DataAsset.h"
#include "DDMiniGameDefinition.generated.h"

/**
 * 
 */
UCLASS()
class DOODOONG_API UDDMiniGameDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	FName MiniGameId = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame", meta=(Categories="MiniGame.Genre"))
	FGameplayTag Genre = DDMiniGameplayTags::Genre_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	float DefaultTimeLimitSeconds = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	FGameplayTagContainer MiniGameTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	TSoftObjectPtr<UWorld> MiniGameMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	TSubclassOf<ADDMiniGameModeBase> GameModeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	TSubclassOf<UDDMiniGameRuleSet> RuleSetClass;

};
