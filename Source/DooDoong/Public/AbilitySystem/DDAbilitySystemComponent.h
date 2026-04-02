// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DDAbilitySystemComponent.generated.h"

class UDDAbilitySet; 

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DOODOONG_API UDDAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	
	void ClearAbilityInput();
	
public:
	void GiveAbilities(UDDAbilitySet* DefaultAbilitySet);
	
protected:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
};
