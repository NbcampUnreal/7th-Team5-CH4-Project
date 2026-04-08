#pragma once

#include "CoreMinimal.h"
#include "DDBasePlayerController.h"
#include "DDLobbyPlayerController.generated.h"

class UUserWidget;

UCLASS()
class ADDLobbyPlayerController : public ADDBasePlayerController
{
	GENERATED_BODY()

public:
	// 클라이언트가 서버에게 닉네임을 전달하는 RPC
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SubmitNickname(const FName& InNickname);

	// 서버가 클라이언트에게 UI를 닫고 게임 모드로 전환하라고 지시하는 RPC
	UFUNCTION(Client, Reliable)
	void Client_JoinLobby();
};
