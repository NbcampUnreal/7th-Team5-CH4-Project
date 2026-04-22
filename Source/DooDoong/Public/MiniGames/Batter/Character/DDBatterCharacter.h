#pragma once

#include "CoreMinimal.h"
#include "Base/Character/DDBaseCharacter.h"
#include "DDBatterCharacter.generated.h"

class UInputAction;

UCLASS()
class DOODOONG_API ADDBatterCharacter : public ADDBaseCharacter
{
	GENERATED_BODY()

public:
	ADDBatterCharacter();

protected:
	virtual void BeginPlay() override;

public:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(Server, Reliable)
	void Server_AddScore();

	UPROPERTY(BlueprintReadOnly)
	bool bPressing;
	
private:
	UPROPERTY(EditAnywhere, Category="Input")
	const UInputAction* IA_Bat;
	
	UFUNCTION()
	void OnPressSpace();
	
	UFUNCTION()
	void OnReleaseSpace();
	
};
