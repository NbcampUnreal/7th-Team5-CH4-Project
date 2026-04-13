#include "Lobby/Game/DDLobbyGameMode.h"
#include "Base/Player/DDBasePlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "TimerManager.h"
#include "Base/Game/DDGameStateBase.h"
#include "Common/DDLogManager.h"
#include "Lobby/Player/DDLobbyPlayerController.h"

ADDLobbyGameMode::ADDLobbyGameMode()
{
	// Seamless Travel 활성화
	bUseSeamlessTravel = true;
}

void ADDLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// 1초마다 대기실 인원 체크 및 카운트다운을 수행하는 타이머 실행
	GetWorld()->GetTimerManager().SetTimer(
		MainTimerHandle, 
		this, 
		&ThisClass::OnMainTimerElapsed, 
		1.f, 
		true
	);
}

void ADDLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ADDLobbyPlayerController* PC = Cast<ADDLobbyPlayerController>(Exiting);
	if (!IsValid(PC)) return;
	
	ADDBasePlayerState* PS = PC->GetPlayerState<ADDBasePlayerState>();
	if (!IsValid(PS)) return;
	
	LOG_CJH(Log, TEXT("[%s]님이 게임을 종료하셨습니다."), *PS->PlayerGameData.PlayerDisplayName.ToString());
	Participants.Remove(PC);
	Spectators.Remove(PC);
}

bool ADDLobbyGameMode::TryRegisterPlayerNickname(ADDLobbyPlayerController* Requester, const FName& Nickname,
	FString& ErrorMessage)
{
	// 1. 유효성 및 중복 체크
	if (!IsValid(Requester))
	{
		ErrorMessage = TEXT("유효하지 않은 컨트롤러 입니다."); 
		return false; 
	}
	
	if (!IsNicknameAvailable(Nickname))
	{
		ErrorMessage = TEXT("중복된 닉네임 입니다."); 
		return false; 
	}
	
	ADDBasePlayerState* PS = Requester->GetPlayerState<ADDBasePlayerState>();
	ADDGameStateBase* GS = GetGameState<ADDGameStateBase>();
	if (!IsValid(PS) || !IsValid(GS))
	{
		ErrorMessage = TEXT("시스템 오류가 발생했습니다."); 
		return false; 
	}
	
	// 2. 닉네임 설정 
	PS->SetPlayerName(Nickname.ToString());
	PS->PlayerGameData.PlayerDisplayName = Nickname;
	
	// 3. 참가자/관전자 판별
	if (Participants.Num() < GS->MinPlayerCount)
	{
		PS->bIsParticipant = true;
		Participants.AddUnique(Requester);
		
		LOG_CJH(Log, TEXT("[%s]님이 게임에 접속하셨습니다."), *Nickname.ToString());
		LOG_CJH(Log, TEXT("낭은 인원 수 (%d / %d)"), Participants.Num() +1, GS->MinPlayerCount);
	}
	else
	{
		PS->bIsParticipant = false;
		Spectators.AddUnique(Requester);
		
		SetPlayerAsSpectator(Requester);
		
		LOG_CJH(Log, TEXT("[%s] 관전자로 등록됨"), *Nickname.ToString());
	} 
	
	Requester->Client_JoinLobby();
	
	return true; 
}

bool ADDLobbyGameMode::IsNicknameAvailable(const FName& InNickname) const
{
	if (!IsValid(GameState)) return false;  
	
	for (APlayerState* PS : GameState->PlayerArray)
	{
		ADDBasePlayerState* DDPS = Cast<ADDBasePlayerState>(PS);
		if (DDPS && DDPS->PlayerGameData.PlayerDisplayName == InNickname)
		{
			LOG_KMS(Warning, TEXT("중복된 닉네임입니다."));
			return false; 
		}
	}
	
	return true; 
}

void ADDLobbyGameMode::SetPlayerAsSpectator(APlayerController* InPlayerController)
{
	if (!IsValid(InPlayerController)) return;
	
	ADDBasePlayerState* PS = InPlayerController->GetPlayerState<ADDBasePlayerState>();
	if (!PS) return;
	
	PS->bIsParticipant = false; 
	
	// 기존 폰 제거 
	if (APawn* CurrentPawn = InPlayerController->GetPawn())
	{
		CurrentPawn->Destroy(); 
	}
	
	// 스폰 위치 결정
	FVector SpectatorLocation = FVector(0, 0, 500);
	if (AActor* StartSpot = ChoosePlayerStart(InPlayerController))
	{
		SpectatorLocation = StartSpot->GetActorLocation() + FVector(0, 0, 300);
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	// 관전자 폰 스폰 및 빙의
	APawn* SpectatorPawn = 
		GetWorld()->SpawnActor<ASpectatorPawn>(
				ASpectatorPawn::StaticClass(),
				SpectatorLocation, 
				FRotator::ZeroRotator, 
				SpawnParams
		);

	if (IsValid(SpectatorPawn))
	{
		InPlayerController->Possess(SpectatorPawn);
	}
}

void ADDLobbyGameMode::OnMainTimerElapsed()
{
	ADDGameStateBase* GS = GetGameState<ADDGameStateBase>();
	if (!GS) return; 
	
	if (!bIsStarting)
	{
		if (Participants.Num() >= GS->MinPlayerCount)
		{
			LOG_CJH(Log, TEXT("곧 게임을 시작합니다."));
			bIsStarting = true;
		}
	}
	else
	{
		if (Participants.Num() < GS->MinPlayerCount)
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
