#pragma once

#include "CoreMinimal.h"
#include "DDInputConfig.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "DDInputComponent.generated.h"

class UDDInputConfig;
class UEnhancedInputLocalPlayerSubsystem;
class UInputAction; 

UCLASS()
class DOODOONG_API UDDInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	UDDInputComponent(const FObjectInitializer& ObjectInitializer); 
	
	void AddInputMapping(const UDDInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubSystem);
	
	void RemoveInputMappings(
		const UDDInputConfig* InputConfig,
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem
	) const;
	
	void RemoveBinds(TArray<uint32>& BindHandles);

public:
	
	template<class UserClass, typename FuncType>
	void BindNativeAction(
		const UDDInputConfig* InputConfig,
		const FGameplayTag& InputTag,
		ETriggerEvent TriggerEvent, 
		UserClass* Object, 
		FuncType Func, 
		bool bLogIfNotFound
	);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(
		const UDDInputConfig* InputConfig,
		UserClass* Object, 
		PressedFuncType PressedFunc, 
		ReleasedFuncType ReleasedFunc,
		TArray<uint32>& BindHandles
	);

};

template <class UserClass, typename FuncType>
void UDDInputComponent::BindNativeAction(const UDDInputConfig* InputConfig, const FGameplayTag& InputTag,
	ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);	
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UDDInputComponent::BindAbilityActions(
	const UDDInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc,
	ReleasedFuncType ReleasedFunc, 
	TArray<uint32>& BindHandles)
{
	check(InputConfig);
	
	for (const FDDInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(
					Action.InputAction,
					Action.PressedTriggerEvent,
					Object,
					PressedFunc, 
					Action.InputTag
				).GetHandle());
			}
			
			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(
					Action.InputAction, 
					Action.ReleasedTriggerEvent,
					Object,
					ReleasedFunc,
					Action.InputTag
				).GetHandle());
			}
		}
	}
}
