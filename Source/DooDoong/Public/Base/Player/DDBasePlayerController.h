#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "DDBasePlayerController.generated.h"

struct FInputActionValue;

class UInputMappingContext;
class UDDInputConfig;

UCLASS()
class DOODOONG_API ADDBasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADDBasePlayerController();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

public:
	void SetInputConfig(UDDInputConfig* NewConfig);

	void SetInputMappingContext(UInputMappingContext* NewIMC);
	
	/** 주현 : GameMode에서 곧바로 Set을 호출하는 것이 불가능해서 추가한 Client RPC */
	UFUNCTION(Client, Reliable)
	void Client_ApplyInput(UDDInputConfig* NewConfig, UInputMappingContext* NewIMC);
	
protected:
	void Input_Move(const FInputActionValue& Value);

	void Input_Look(const FInputActionValue& Value);

	void Input_AbilityPressed(FGameplayTag InputTag);

	void Input_AbilityReleased(FGameplayTag InputTag);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UDDInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultIMC;
};
