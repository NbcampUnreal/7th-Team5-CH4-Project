// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DDItemDataTable.generated.h"

class UImage;

USTRUCT(BlueprintType)
struct FDDItemDataTable : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item|Data")
	FName ItemName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item|Data")
	UImage* Image;
};