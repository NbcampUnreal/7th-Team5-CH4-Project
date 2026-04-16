#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "DDItemDataTypes.generated.h"

class UGameplayAbility;

USTRUCT(BlueprintType)
struct DOODOONG_API FItemTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	/** 아이템 ID */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemID;
	
	/** 표시될 아이템 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;
	
	/** 표시될 아이템 설명 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
	
	/** 아이템 타입 태그 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ItemTag;
	
	/** 아이템 어빌리티 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AbilityTag;
	
	/** 표시될 아이템 아이콘 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;
	
	/** 아이템을 사용하면 갈아끼울 메쉬 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> DisplayActor;
};