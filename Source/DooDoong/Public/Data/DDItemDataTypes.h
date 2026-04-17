#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "DDItemDataTypes.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct DOODOONG_API FGameplayCueData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag CueTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> Effect;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> Sound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> ActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName AttachSocketName;
};


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
	
	/** 실행할 아이템 어빌리티 태그 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ItemAbilityTag;
	
	/** 표시될 아이템 아이콘 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;
	
	/** GameplayCue에 사용될 데이터 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayCueData CueData;
};
