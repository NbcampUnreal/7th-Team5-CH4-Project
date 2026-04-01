#pragma once

#include "CoreMinimal.h"
#include "Base/Character/DDBaseCharacter.h"
#include "DDBoardGameCharacter.generated.h"

class UDDBoardGameAttributeSet; 

UCLASS()
class DOODOONG_API ADDBoardGameCharacter : public ADDBaseCharacter
{
	GENERATED_BODY()
	
public:
	ADDBoardGameCharacter(); 
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDDBoardGameAttributeSet> AttributeSet;
};
