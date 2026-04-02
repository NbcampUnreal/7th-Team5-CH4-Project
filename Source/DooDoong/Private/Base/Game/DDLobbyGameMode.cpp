#include "Base/Game/DDLobbyGameMode.h"
#include "Base/Player/DDLobbyPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "System/DDGameInstance.h"
#include "TimerManager.h"

void ADDLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	bUseSeamlessTravel = true;
	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.f, true);

	// 로비 맵 진입 시 이전 게임 명단 초기화
	if (UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance()))
	{
		GameInstance->ParticipantNames.Empty();
	}
}

void ADDLobbyGameMode::ProcessPlayerJoin(ADDLobbyPlayerController* LobbyPlayerController, const FString& Nickname)
{
	if (!IsValid(LobbyPlayerController)) return;

	if (APlayerState* PlayerState = LobbyPlayerController->GetPlayerState<APlayerState>())
	{
		PlayerState->SetPlayerName(Nickname);
	}

	// 선착순 4명 (참여자)
	if (Participants.Num() < 4)
	{
		if (UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance()))
		{
			GameInstance->ParticipantNames.Add(Nickname);
		}

		Participants.Add(LobbyPlayerController);
		LobbyPlayerController->Client_JoinLobby();
	}
	// 이후 플레이어 (관전자 강등)
	else
	{
		// 늦게 들어온 유저의 기존 폰 제거
		if (APawn* CurrentPawn = LobbyPlayerController->GetPawn())
		{
			CurrentPawn->Destroy();
		}

		// 관전자 전용 폰 스폰 및 빙의
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SpectatorLocation = FVector(0, 0, 500);
		if (AActor* StartSpot = ChoosePlayerStart(LobbyPlayerController))
		{
			SpectatorLocation = StartSpot->GetActorLocation() + FVector(0, 0, 300);
		}

		APawn* Spectator = GetWorld()->SpawnActor<ASpectatorPawn>(ASpectatorPawn::StaticClass(), SpectatorLocation,
		                                                          FRotator::ZeroRotator, SpawnParams);
		if (Spectator)
		{
			LobbyPlayerController->Possess(Spectator);
		}

		Spectators.Add(LobbyPlayerController);
		LobbyPlayerController->Client_JoinLobby();
	}
}

void ADDLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ADDLobbyPlayerController* ExitingPlayerController = Cast<ADDLobbyPlayerController>(Exiting);
	if (IsValid(ExitingPlayerController))
	{
		Participants.Remove(ExitingPlayerController);
		Spectators.Remove(ExitingPlayerController);
	}
}

void ADDLobbyGameMode::OnMainTimerElapsed()
{
	if (!bIsStarting)
	{
		if (Participants.Num() >= 4)
		{
			bIsStarting = true;
			RemainWaitingTimeForStarting = 5;
		}
	}
	else
	{
		// 카운트다운 중 누군가 나가서 4명 미만이 되면 시작 취소
		if (Participants.Num() < 4)
		{
			bIsStarting = false;
			return;
		}

		--RemainWaitingTimeForStarting;

		if (RemainWaitingTimeForStarting <= 0)
		{
			GetWorld()->GetTimerManager().ClearTimer(MainTimerHandle);
			GetWorld()->ServerTravel(BaseGameMapPath);
		}
	}
}
