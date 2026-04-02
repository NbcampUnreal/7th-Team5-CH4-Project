#include "Base/Game/DDGameModeBase.h"
#include "Base/Game/DDGameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "System/DDGameInstance.h"
#include "TimerManager.h"

void ADDGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	ADDGameStateBase* DDGameState = GetGameState<ADDGameStateBase>();
	if (IsValid(DDGameState))
	{
		DDGameState->MatchState = EDDMatchState::Playing;
	}

	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.f, true);
}

void ADDGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (IsValid(NewPlayer))
	{
		AlivePlayerControllers.Add(NewPlayer);
	}
}

void ADDGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerController* ExitingPlayerController = Cast<APlayerController>(Exiting);
	if (IsValid(ExitingPlayerController))
	{
		AlivePlayerControllers.Remove(ExitingPlayerController);
	}
}

void ADDGameModeBase::HandleSeamlessTravelPlayer(AController*& ParticipantController)
{
	Super::HandleSeamlessTravelPlayer(ParticipantController);

	// 트래블 직후 플레이어가 보유한 기존 폰 파괴 (초기화)
	if (APawn* OldPawn = ParticipantController->GetPawn())
	{
		OldPawn->Destroy();
	}

	// 명단 대조 진행
	if (APlayerState* PlayerState = ParticipantController->GetPlayerState<APlayerState>())
	{
		FString MyName = PlayerState->GetPlayerName();
		if (UDDGameInstance* GI = Cast<UDDGameInstance>(GetGameInstance()))
		{
			// 명단에 없는 유저일 경우 관전자로 강등
			if (!GI->ParticipantNames.Contains(MyName))
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				APawn* Spectator = GetWorld()->SpawnActor<ASpectatorPawn>(
					ASpectatorPawn::StaticClass(), FVector(0, 0, 1000), FRotator::ZeroRotator, SpawnParams);

				if (Spectator) ParticipantController->Possess(Spectator);

				return; // 함수 조기 종료 (캐릭터 스폰 스킵)
			}
		}
	}

	// 명단에 있는 참여자라면 기본 스폰 로직 실행
	RestartPlayer(ParticipantController);
}

void ADDGameModeBase::OnMainTimerElapsed()
{
	ADDGameStateBase* DDGameState = GetGameState<ADDGameStateBase>();
	if (!IsValid(DDGameState)) return;

	switch (DDGameState->MatchState)
	{
	case EDDMatchState::Playing:
		// 추후 생존자 체크 로직 구현 예정
		break;

	case EDDMatchState::Ending:
		// 추후 게임 종료 로직 구현 예정
		break;

	default:
		break;
	}
}
