#pragma once
#include "CoreMinimal.h"
#include "Base/Player/DDBasePlayerController.h"
#include "DDLobbyPlayerController.generated.h"

class UUserWidget;

UCLASS()
class ADDLobbyPlayerController : public ADDBasePlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	// 클라이언트가 서버에게 닉네임을 전달하는 RPC
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SubmitNickname(const FName& InNickname);

	// 서버가 클라이언트에게 UI를 닫고 게임 모드로 전환하라고 지시하는 RPC
	UFUNCTION(Client, Reliable)
	void Client_JoinLobby();
	
	// 중복 닉네임인 경우 실패 통보할 RPC
    UFUNCTION(Client, Reliable)
    void Client_ReceiveNicknameFailure(const FString& ErrorMessage);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestReady(bool bIsReady); 
	
public:
	// 입력 모드 토글 함수 (UIOnly <-> GameOnly) 
	void ToggleUIInputMode(bool bUIOnly);	
	
protected: 
	// 로비 UI 
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> LobbyWidgetClass; 
};
