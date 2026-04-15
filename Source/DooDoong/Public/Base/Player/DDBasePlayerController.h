#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BoardGame/DDSelectableTileActor.h"
#include "GameFramework/PlayerController.h"
#include "DDBasePlayerController.generated.h"

class UDDInventoryComponent;
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
	void SetInputMappingContext(UInputMappingContext* NewIMC);

	/** 윤서 : GameMode에서 상태(로비/보드)에 따라 IMC 지정 */
	UFUNCTION(Client, Reliable)
	void Client_ApplyState(FGameplayTag StateTag);

	/** 주현 : GameMode에서 곧바로 Set을 호출하는 것이 불가능해서 추가한 Client RPC */
	UFUNCTION(Client, Reliable)
	void Client_ApplyInput(UInputMappingContext* NewIMC);

	/** 주현 : 준비완료 상태를 서버에서 호출시키기 위한 Server RPC */
	UFUNCTION(Server, Reliable)
	void Server_SetMiniGameReady(bool bReady);
	
	/** 준혁 : BoardGameMode에서 플레이어 턴에 맞는 InputMode를 적용하기 위한 Client RPC */
	UFUNCTION(Client, Reliable)
	void Client_SetMouseCursorVisible(bool bVisible);

	void OnMouseClick();

	UFUNCTION(Server, Reliable)
	void Server_SelectTile(ADDSelectableTileActor* TileActor);
	
public:
	UFUNCTION(Server, Reliable)
	void Server_RequestPlayerReady(); 

protected:
	void Input_Move(const FInputActionValue& Value);

	void Input_Look(const FInputActionValue& Value);

	void Input_AbilityPressed(FGameplayTag InputTag);

	void Input_AbilityReleased(FGameplayTag InputTag);
	
	void ToggleInventoryMenu();
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UDDInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultIMC;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> BoardGameIMC;
	
private:
	TWeakObjectPtr<UDDInventoryComponent> InventoryComponent;
};
