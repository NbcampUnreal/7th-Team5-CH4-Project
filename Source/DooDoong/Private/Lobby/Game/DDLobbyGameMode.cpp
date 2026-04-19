#include "Lobby/Game/DDLobbyGameMode.h"
#include "Base/Player/DDBasePlayerState.h"
#include "Lobby/Game/DDLobbyGameState.h"
#include "Lobby/Player/DDLobbyPlayerController.h"
#include "GameFramework/SpectatorPawn.h"
#include "Common/DDLogManager.h"
#include "System/DDGameInstance.h"
#include "System/DDGameplayTags.h"


ADDLobbyGameMode::ADDLobbyGameMode()
{
	// Seamless Travel 활성화
	bUseSeamlessTravel = true;
}

void ADDLobbyGameMode::InitGameState()
{
	Super::InitGameState();
	
	CachedGameState = GetGameState<ADDLobbyGameState>();
	LOG_KMS(Error, TEXT("CachedGameState: %s"), *GetNameSafe(CachedGameState));
}

void ADDLobbyGameMode::GenericPlayerInitialization(AController* C)
{
	Super::GenericPlayerInitialization(C);
	
	// 1. 플레이어 입장 
	ADDLobbyPlayerController* LobbyPC = Cast<ADDLobbyPlayerController>(C); 
	if (!IsValid(LobbyPC)) return;
	
	// 2. UI 그리기 
	if (LobbyUIConfig)
	{
		// 메인 UI 
		LobbyPC->Client_SetUIConfig(LobbyUIConfig); 
		
		// 닉네임 입력창
		LobbyPC->Client_OpenPopUp(DDGameplayTags::Lobby_UI_NicknamePopup); 
	}
}

void ADDLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ADDLobbyPlayerController* PC = Cast<ADDLobbyPlayerController>(Exiting);
	if (!IsValid(PC)) return;
	
	ADDBasePlayerState* PS = PC->GetPlayerState<ADDBasePlayerState>();
	if (!IsValid(PS)) return;
	
	LOG_CJH(Log, TEXT("[%s]님이 게임을 종료하셨습니다."), *PS->PlayerGameData.PlayerDisplayName.ToString());
	
	// 나간 플레이어 색 반납 
	FLinearColor FreedColor = PS->PlayerGameData.PlayerColor;
    if (FreedColor != FLinearColor::White)
    {
        AvailableColors.AddUnique(FreedColor);
    }
	
	Participants.Remove(PC);
	GetLobbyGameState()->PlayerCount = Participants.Num();
	
	Spectators.Remove(PC);
}

bool ADDLobbyGameMode::TryRegisterPlayerNickname(
	ADDLobbyPlayerController* Requester, 
	const FName& Nickname,
	FString& ErrorMessage)
{
	// 1. 유효성 및 중복 체크
	if (!IsValid(Requester))
	{
		ErrorMessage = TEXT("유효하지 않은 컨트롤러 입니다."); 
		return false; 
	}
	
	if (!IsNicknameAvailable(Nickname, ErrorMessage))
	{
		return false;
	}
	
	ADDBasePlayerState* PS = Requester->GetPlayerState<ADDBasePlayerState>();
	if (!IsValid(PS))
	{
		ErrorMessage = TEXT("시스템 오류가 발생했습니다. : PlayerState가 유효하지 않음."); 
		return false; 
		
	}
	
	if (!IsValid(GetLobbyGameState()))
	{
		ErrorMessage = TEXT("시스템 오류가 발생했습니다. GameState가 유효하지않음."); 
		return false; 
	}
	
	// 2. 닉네임 설정 
	PS->SetPlayerName(Nickname.ToString());
	PS->PlayerGameData.PlayerDisplayName = Nickname;
	
	// 3. 캐릭터 색상 설정
	FLinearColor NewRandomColor = GetRandomAvailableColor();
	PS->SetPlayerColor(NewRandomColor);
	
	// 4. 참가자/관전자 판별
	if (Participants.Num() < CachedGameState->MaxPlayerCount)
	{
		PS->bIsParticipant = true;
		Participants.AddUnique(Requester);
		CachedGameState->PlayerCount = Participants.Num(); 
		
		LOG_CJH(Log, TEXT("[%s]님이 게임에 접속하셨습니다."), *Nickname.ToString());
		LOG_CJH(Log, TEXT("현재 인원 수 (%d / %d) [게임 최소 인원 수: %d]"), Participants.Num(), CachedGameState->MaxPlayerCount, CachedGameState->RequiredPlayerCount);
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

void ADDLobbyGameMode::RequestPlayerReady(ADDLobbyPlayerController* Requester, bool bReady)
{
	if (!Participants.Contains(Requester)) return;
	
	if(!CachedGameState) return;
	
	if (bReady)
	{
		ReadyParticipants.AddUnique(Requester);
		CachedGameState->ReadyCount++;
		
		if (ReadyParticipants.Num() == Participants.Num() &&
			ReadyParticipants.Num() >= GetLobbyGameState()->RequiredPlayerCount)
		{
			StartPlayCountDown(StartCountdownSeconds); 
		}
	}
	else
	{
		ReadyParticipants.Remove(Requester);
		CachedGameState->ReadyCount--;
		
		GetWorld()->GetTimerManager().ClearTimer(StartTimerHandle);
		
		for (auto PC : Participants)
		{
			PC->Client_ClosePopUp(DDGameplayTags::Lobby_UI_CountDown);
		}
	}
}

void ADDLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	AvailableColors = DefaultPlayerColors;
}

ADDLobbyGameState* ADDLobbyGameMode::GetLobbyGameState()
{ 
	if (!CachedGameState) CachedGameState = GetGameState<ADDLobbyGameState>();
	
	return CachedGameState;
}

bool ADDLobbyGameMode::IsNicknameAvailable(const FName& InNickname, FString& ErrorMessage) 
{
	if (!IsValid(GetLobbyGameState()))
	{
		ErrorMessage = TEXT("시스템 오류가 발생했습니다. 오류 : GameState가 유효하지 않음."); 
		return false;
	}  
	
	if (InNickname.IsNone() || InNickname.ToString().Len() > 6)
	{
		ErrorMessage = TEXT("닉네임은 1자 이상, 6자 이하로 입력해주세요.");
		return false;
	}
	
	for (APlayerState* PS : GameState->PlayerArray)
	{
		ADDBasePlayerState* DDPS = Cast<ADDBasePlayerState>(PS);
		if (DDPS && DDPS->PlayerGameData.PlayerDisplayName == InNickname)
		{
			ErrorMessage = TEXT("중복된 닉네임입니다.");
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

void ADDLobbyGameMode::StartPlayCountDown(float InSeconds)
{
	GetLobbyGameState()->CountDown = InSeconds;
	LOG_CJH(Log, TEXT("곧 게임을 시작합니다."));
	
	// 카운트다운 ui 열기 
	for (auto PC : Participants)
	{
		PC->Client_OpenPopUp(DDGameplayTags::Lobby_UI_CountDown); 
	}
	
	// 1초마다 대기실 인원 체크 및 카운트다운을 수행하는 타이머 실행
	GetWorld()->GetTimerManager().SetTimer(
		StartTimerHandle, 
		this, 
		&ThisClass::OnMainTimerElapsed, 
		1.f, 
		true
	);
}

void ADDLobbyGameMode::OnMainTimerElapsed()
{
	if (!IsValid(GetLobbyGameState())) return; 
	
	CachedGameState->CountDown--;
	LOG_CJH(Log, TEXT("%d초 후 게임에 진입합니다."), CachedGameState->CountDown);
	
	if (CachedGameState->CountDown <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(StartTimerHandle);
		LOG_CJH(Log, TEXT("보드게임에 진입합니다."));
		
		TryStartBoardGame(); 
	}
}

bool ADDLobbyGameMode::CanStartBoardGame(FString& ErrorMessage)
{
	for (auto PC : Participants)
	{
		PC->Client_ClosePopUp(DDGameplayTags::Lobby_UI_CountDown);
	}
	
	if (!IsValid(GetLobbyGameState()))
	{
		ErrorMessage = TEXT("시스템 오류가 발생했습니다. 오류 : GameState가 유효하지 않음.");
		return false; 
	}
	
	if (Participants.Num() < CachedGameState->RequiredPlayerCount)
	{
		ErrorMessage = FString::Printf(
			TEXT("시작할려면 최소 %d 이상 되어야 합니다."),
			CachedGameState->RequiredPlayerCount
		);
		
		return false; 
	}
	
	if (ReadyParticipants.Num() < CachedGameState->RequiredPlayerCount)
	{
		ErrorMessage = FString::Printf(
		TEXT("모든 플레이어가 준비해야 합니다. (준비 %d / %d명)"),
			ReadyParticipants.Num(),
			Participants.Num()
		);
		
		return false;
	}
	
	return true; 
}

void ADDLobbyGameMode::TryStartBoardGame()
{
	FString ErrorMessage; 
	if (!CanStartBoardGame(ErrorMessage))
	{
		LOG_KMS(Warning, TEXT("%s"), *ErrorMessage);
		return; 
	}
	
	LOG_KMS(Log, TEXT("게임이 시작됩니다.")); 
	
	UDDGameInstance* GameInstance = Cast<UDDGameInstance>(GetGameInstance());
	GameInstance->ExpectedPlayerCount = ReadyParticipants.Num();
	
	GetWorld()->ServerTravel(BoardGameMapPath);
}

FLinearColor ADDLobbyGameMode::GetRandomAvailableColor()
{
	if (AvailableColors.IsEmpty())
    {
        return FLinearColor::White; 
    }

    // 1. 남은 색상들 중에서 랜덤한 인덱스 번호를 하나 뽑습니다.
    int32 RandomIndex = FMath::RandRange(0, AvailableColors.Num() - 1);

    // 2. 해당 인덱스의 색상을 변수에 임시 저장합니다.
    FLinearColor SelectedColor = AvailableColors[RandomIndex];

    // 3. 누군가 이 색상을 가져갔으므로, 목록에서 영구히 삭제하여 중복을 방지합니다.
    AvailableColors.RemoveAt(RandomIndex);

    return SelectedColor;
}
