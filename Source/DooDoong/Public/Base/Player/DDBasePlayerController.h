#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BoardGame/DDSelectableTileActor.h"
#include "GameFramework/PlayerController.h"
#include "Base/Player/DDBasePlayerState.h"
#include "DDBasePlayerController.generated.h"

class ADDBasePlayerState;
struct FInputActionValue;

class UInputMappingContext;
class UDDInputConfig;
class UDDInventoryComponent;
class UItemActionComponent;
class UDDInventoryWidget;
class UDDUIConfig;

UCLASS()
class DOODOONG_API ADDBasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADDBasePlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupInputComponent() override;

protected:
	UPROPERTY()
	ADDBasePlayerState* CachedPlayerState;

public:
	FORCEINLINE ADDBasePlayerState* GetCachedPlayerState() 
	{
		if (!CachedPlayerState) CachedPlayerState = GetPlayerState<ADDBasePlayerState>();
		return CachedPlayerState;
	}

public:
	void SetInputMappingContext(UInputMappingContext* NewIMC);

	// GameMode가 상태 변경을 클라이언트에게 전달하는 RPC들
	
	/** 윤서 : GameMode에서 상태(로비/보드)에 따라 IMC 지정 */
	UFUNCTION(Client, Reliable)
	void Client_ApplyState(FGameplayTag StateTag);

	/** 주현 : GameMode에서 곧바로 Set을 호출하는 것이 불가능해서 추가한 Client RPC */
	UFUNCTION(Client, Reliable)
	void Client_ApplyInput(UInputMappingContext* NewIMC);

	/** 준혁 : BoardGameMode에서 플레이어 턴에 맞는 InputMode를 적용하기 위한 Client RPC */
	UFUNCTION(Client, Reliable)
	void Client_SetMouseCursorVisible(bool bVisible);
	
	/** 주현 : 준비완료 상태를 서버에서 호출시키기 위한 Server RPC */
	UFUNCTION(Server, Reliable)
	void Server_SetMiniGameReady(bool bReady);
	
	UFUNCTION(Server, Reliable)
	void Server_RequestPlayerReady(); 

	UFUNCTION(Server, Reliable)
	void Server_SelectTile(ADDSelectableTileActor* TileActor);
	
	UFUNCTION(Server, Reliable)
	void Server_NotifyClientLoaded();
	
	void OnMouseClick();

public:
	// UI 및 팝업 관련 RPC들
	UFUNCTION(Client, Reliable)
	void Client_SetUIConfig(UDDUIConfig* InConfig);

	UFUNCTION(Client, Reliable)
	void Client_SetUIConfigByPath(const FSoftObjectPath& InConfigPath);
	
	UFUNCTION(Client, Reliable)
	void Client_SwitchGameLayer(FGameplayTag Tag);
	
	UFUNCTION(Client, Reliable)
	void Client_OpenPopUp(FGameplayTag Tag);
	
	UFUNCTION(Client, Reliable)
	void Client_ClosePopUp(FGameplayTag Tag);
	
	UFUNCTION(Client, Reliable)
	void Client_CloseAllPopUps();
	
	UFUNCTION(Client, Reliable)
	void Client_DrawErrorMessage(const FString& ErrorMessage, float Duration = 3.f);

protected:
	void Input_Move(const FInputActionValue& Value);

	void Input_Look(const FInputActionValue& Value);

	void Input_AbilityPressed(FGameplayTag InputTag);

	void Input_AbilityReleased(FGameplayTag InputTag);

protected:
	/** 주현 : ItemActionComponent 관련 */
	
	/** 다음 타겟 선택 */
	void Input_ItemNextTarget();

	/** 이전 타겟 선택 */
	void Input_ItemPreviousTarget();

	/** 아이템 사용 확정 */
	void Input_ItemConfirm();

	/** 아이템 사용 취소 */
	void Input_ItemCancel();

	/** 아이템 액션 컴포넌트를 매번 가져올 때 방어코드 작성하는게 코드를 지저분하게 만들어서 만든 헬퍼 */
	UItemActionComponent* GetItemActionComponentFromPawn() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UDDInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultIMC;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> BoardGameIMC;
	
	/*민재 : 인벤토리 코드*/

public:
	UDDInventoryComponent* GetInventoryComponent() const {return InventoryComponent;}
	
protected:
	void Input_ToggleInventoryMenu();
	
	UFUNCTION(Server, Reliable)
	void Server_RequestInventory();

private:
	UPROPERTY(VisibleAnywhere, Category="Inventory")
	TObjectPtr<UDDInventoryComponent> InventoryComponent;
	
	bool bInventoryOpen;
};
