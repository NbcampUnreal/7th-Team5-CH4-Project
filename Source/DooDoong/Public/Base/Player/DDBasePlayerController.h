#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "DDBasePlayerController.generated.h"

struct FInputActionValue;

class UInputMappingContext;
class UDDInputConfig;
class UMiniGameUIWrapper;

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
    void SetInputMappingContext(UInputMappingContext* NewIMC);

    /** MiniGame UI */
    UPROPERTY()
    UMiniGameUIWrapper* MiniGameUI;

    /** Ready UI RPCs */
    UFUNCTION(Client, Reliable)
    void Client_OpenReadyUI();

    UFUNCTION(Client, Reliable)
    void Client_CloseReadyUI();

    /** GameState 기반 InputContext 적용 */
    UFUNCTION(Client, Reliable)
    void Client_ApplyState(FGameplayTag StateTag);

    /** Client RPC를 통해 InputContext 적용 */
    UFUNCTION(Client, Reliable)
    void Client_ApplyInput(UInputMappingContext* NewIMC);

    /** 서버에서 Ready 상태 설정 */
    UFUNCTION(Server, Reliable, BlueprintCallable)
    void Server_SetMiniGameReady(bool bReady);

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

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> BoardGameIMC;
};