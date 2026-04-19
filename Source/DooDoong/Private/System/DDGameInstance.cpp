// Fill out your copyright notice in the Description page of Project Settings.


#include "System/DDGameInstance.h"

#include "Kismet/GameplayStatics.h"

UDDGameInstance* UDDGameInstance::Get(const UObject* WorldContext)
{
	return Cast<UDDGameInstance>(UGameplayStatics::GetGameInstance(WorldContext));
}
