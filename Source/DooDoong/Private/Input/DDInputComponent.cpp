#include "Input/DDInputComponent.h"

UDDInputComponent::UDDInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UDDInputComponent::AddInputMapping(const UDDInputConfig* InputConfig,
	UEnhancedInputLocalPlayerSubsystem* InputSubSystem)
{
	check(InputConfig);
	check(InputSubSystem);
}

void UDDInputComponent::RemoveInputMappings(const UDDInputConfig* InputConfig,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);
}

void UDDInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	
	BindHandles.Reset();
}
