#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DDUIConfig.generated.h"


UCLASS()
class DOODOONG_API UDDUIConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Layer|Game")
	TMap<FGameplayTag, TSubclassOf<UUserWidget>> StateWidgetMap;
	
	UPROPERTY(EditDefaultsOnly, Category = "Layer|Overlay")
	TSubclassOf<UUserWidget> DefaultOverlayClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Layer|Popup")
	TMap<FGameplayTag, TSubclassOf<UUserWidget>> PopupWidgetMap;
};
