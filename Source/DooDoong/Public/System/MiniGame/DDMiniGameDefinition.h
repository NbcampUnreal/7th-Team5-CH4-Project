#pragma once

#include "CoreMinimal.h"
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MiniGame")
	FName MiniGameId = NAME_None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MiniGame")
	FText DisplayName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MiniGame")
	FText Description;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniGame")
	float DefaultTimeLimitSeconds = 60.f;

};
