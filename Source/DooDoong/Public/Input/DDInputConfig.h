#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DDInputConfig.generated.h"

USTRUCT(BlueprintType)
struct FDDInputAction
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Category = "InputTag"))
	FGameplayTag InputTag; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Category = "InputTag"))
	ETriggerEvent PressedTriggerEvent = ETriggerEvent::Triggered;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Category = "InputTag"))
	ETriggerEvent ReleasedTriggerEvent = ETriggerEvent::Completed;
};

UCLASS()
class DOODOONG_API UDDInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UDDInputConfig(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "Pawn")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FDDInputAction> NativeInputActions;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FDDInputAction> AbilityInputActions;
};
