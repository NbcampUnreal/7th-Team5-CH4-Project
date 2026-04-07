#include "Base/Game/DDLobbyGameMode.h"
#include "Base/Player/DDLobbyPlayerController.h"
#include "Base/Player/DDBasePlayerState.h" // ★ 공통 PlayerState 포함
#include "GameFramework/SpectatorPawn.h"
#include "TimerManager.h"

void ADDLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 심리스 트래블(Seamless Travel) 활성화
	bUseSeamlessTravel = true;

	// 1초마다 대기실 인원 체크 및 카운트다운을 수행하는 타이머 실행
	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.f, true);
}

void ADDLobbyGameMode::ProcessPlayerJoin(ADDLobbyPlayerController* LobbyPlayerController, const FString& Nickname)
{
	if (!IsValid(LobbyPlayerController)) return;

	// 플레이어 스테이트를 가져와 닉네임 및 참여 권한(Data)을 설정합니다.
	if (ADDBasePlayerState* BasePlayerState = LobbyPlayerController->GetPlayerState<ADDBasePlayerState>())
	{
		// 엔진 기본 닉네임과 커스텀 닉네임 변수 동기화
		BasePlayerState->SetPlayerName(Nickname);
		BasePlayerState->Nickname = Nickname;

		// 선착순 4명 이내일 경우 참여자(Participant)로 설정
		if (Participants.Num() < 4)
		{
			// GameInstance 명단 배열 대신 개인의 PlayerState에 VIP 티켓 발급
			BasePlayerState->bIsParticipant = true;
			Participants.AddUnique(LobbyPlayerController);

			LobbyPlayerController->Client_JoinLobby();
		}
		// 정원이 초과된 경우 관전자(Spectator)로 강등
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

			Spectators.AddUnique(LobbyPlayerController);
			LobbyPlayerController->Client_JoinLobby();
		}
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
	// 4명이 모이지 않았다면 카운트다운 대기
	if (!bIsStarting)
	{
		if (Participants.Num() >= RemainPerson)
		{
			bIsStarting = true;
			RemainWaitingTimeForStarting = 5; // 5초 카운트다운 세팅
		}
	}
	else
	{
		// 카운트다운 도중 플레이어가 이탈하여 4명 미만이 되면 시작 취소
		if (Participants.Num() < RemainPerson)
		{
			bIsStarting = false;
			return;
		}

		--RemainWaitingTimeForStarting;

		// 카운트다운 종료 시 본 게임으로 심리스 트래블 실행
		if (RemainWaitingTimeForStarting <= 0)
		{
			GetWorld()->GetTimerManager().ClearTimer(MainTimerHandle);
			GetWorld()->ServerTravel(BaseGameMapPath);
		}
	}
}
