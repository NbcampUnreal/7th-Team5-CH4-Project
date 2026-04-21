// Fill out your copyright notice in the Description page of Project Settings.


#include "System/DDGameInstance.h"
#include "Abilities/GameplayAbility.h"
#include "Kismet/GameplayStatics.h"

void UDDGameInstance::Init()
{
	Super::Init();

	int32 ValidCount = 0;
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : PreloadedAbilityClasses)
	{
		if (*AbilityClass) { ++ValidCount; }
	}
}

UDDGameInstance* UDDGameInstance::Get(const UObject* WorldContext)
{
	return Cast<UDDGameInstance>(UGameplayStatics::GetGameInstance(WorldContext));
}
