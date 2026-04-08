#include "Base/Game/DDLobbyGameMode.h"
#include "Base/Player/DDLobbyPlayerController.h"
#include "Base/Player/DDBasePlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "TimerManager.h"
#include "Base/Game/DDGameStateBase.h"
#include "Common/DDLogManager.h"

void ADDLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Seamless Travel 활성화
	bUseSeamlessTravel = true;

	// 1초마다 대기실 인원 체크 및 카운트다운을 수행하는 타이머 실행
	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.f, true);
}

void ADDLobbyGameMode::ProcessPlayerJoin(ADDLobbyPlayerController* LobbyPlayerController, const FName& Nickname)
{
	if (!IsValid(LobbyPlayerController)) return;
	
	if (ADDBasePlayerState* BasePlayerState = LobbyPlayerController->GetPlayerState<ADDBasePlayerState>())
	{
		// 닉네임 설정
		BasePlayerState->PlayerGameData.PlayerDisplayName = Nickname;

		ADDGameStateBase* GameStateBase = GetGameState<ADDGameStateBase>();
		// 선착순 N명 이내일 경우 플레이어로 설정
		if (Participants.Num() < GameStateBase->MinPlayerCount)
		{
			BasePlayerState->bIsParticipant = true;
			
			LOG_CJH(Log, TEXT("[%s]님이 게임에 접속하셨습니다. 게임에 필요한 인원 수: %d/%d"), 
				*BasePlayerState->PlayerGameData.PlayerDisplayName.ToString(), 
				Participants.Num() + 1, 
				GameStateBase->MinPlayerCount);
			
			Participants.AddUnique(LobbyPlayerController);
			LobbyPlayerController->Client_JoinLobby();
		}
		// 정원이 초과된 경우 관전자로 강등
		else
		{
			BasePlayerState->bIsParticipant = false;

			// 늦게 들어온 유저의 기존 기본 폰 제거
			if (APawn* CurrentPawn = LobbyPlayerController->GetPawn())
			{
				CurrentPawn->Destroy();
			}

			// 관전자 전용 폰 스폰 (충돌 무시 설정)
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FVector SpectatorLocation = FVector(0, 0, 500);
			if (AActor* StartSpot = ChoosePlayerStart(LobbyPlayerController))
			{
				SpectatorLocation = StartSpot->GetActorLocation() + FVector(0, 0, 300);
			}

			APawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(
				ASpectatorPawn::StaticClass(), SpectatorLocation, FRotator::ZeroRotator, SpawnParams);

			if (IsValid(SpectatorPawn))
			{
				LobbyPlayerController->Possess(SpectatorPawn);
			}

			LOG_CJH(Log, TEXT("[%s]님이 관전자로 접속하셨습니다."), *BasePlayerState->PlayerGameData.PlayerDisplayName.ToString());
			Spectators.AddUnique(LobbyPlayerController);
			LobbyPlayerController->Client_JoinLobby();
		}
	}
}

void ADDLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ADDLobbyPlayerController* ExitingPlayerController = Cast<ADDLobbyPlayerController>(Exiting);
	ADDBasePlayerState* BasePlayerState = ExitingPlayerController->GetPlayerState<ADDBasePlayerState>();
	if (IsValid(ExitingPlayerController))
	{
		LOG_CJH(Log, TEXT("[%s]님이 게임을 종료하셨습니다."), *BasePlayerState->PlayerGameData.PlayerDisplayName.ToString());
		Participants.Remove(ExitingPlayerController);
		Spectators.Remove(ExitingPlayerController);
	}
}

void ADDLobbyGameMode::OnMainTimerElapsed()
{
	ADDGameStateBase* GameStateBase = GetGameState<ADDGameStateBase>();
	
	if (!bIsStarting)
	{
		if (Participants.Num() >= GameStateBase->MinPlayerCount)
		{
			LOG_CJH(Log, TEXT("곧 게임을 시작합니다."));
			bIsStarting = true;
		}
	}
	else
	{
		if (Participants.Num() < GameStateBase->MinPlayerCount)
		{
			LOG_CJH(Log, TEXT("플레이어가 이탈하여 게임 시작이 취소되었습니다."));
			bIsStarting = false;
			WaitingTime = MaxWaitingTime;
			return;
		}

		LOG_CJH(Log, TEXT("%d초 후 게임에 진입합니다."), WaitingTime);
		--WaitingTime;
		
		if (WaitingTime <= 0)
		{
			GetWorld()->GetTimerManager().ClearTimer(MainTimerHandle);
			LOG_CJH(Log, TEXT("보드게임에 진입합니다."), WaitingTime);
			
			GetWorld()->ServerTravel(BaseGameMapPath);
		}
	}
}
