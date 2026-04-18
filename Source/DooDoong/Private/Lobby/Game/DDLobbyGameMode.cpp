#include "Lobby/Game/DDLobbyGameMode.h"
#include "Base/Player/DDBasePlayerState.h"
#include "Lobby/Player/DDLobbyPlayerController.h"
#include "GameFramework/SpectatorPawn.h"
#include "Common/DDLogManager.h"
#include "Algo/Count.h"
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
	
	// Game Sate 캐싱 
	CachedGameState = GetGameState<ADDLobbyGameState>();
}

void ADDLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	if (!IsValid(GetLobbyGameState())) return;
	
	ADDLobbyPlayerController* PC = Cast<ADDLobbyPlayerController>(Exiting);
	if (!IsValid(PC)) return;
	
	if (ADDBasePlayerState* PS = PC->GetPlayerState<ADDBasePlayerState>())
	{
		LOG_CJH(Log, TEXT("[%s]님이 게임을 종료하셨습니다."),
			*PS->PlayerGameData.PlayerDisplayName.ToString());

		if (PS->PlayerGameData.PlayerColor != FLinearColor::White)
			AvailableColors.AddUnique(PS->PlayerGameData.PlayerColor);
	}

	Participants.RemoveAll([PC](const FLobbyParticipantInfo& P){ return P.Controller == PC; });
	CachedGameState->PlayerCount = GetParticipantCount();
}

void ADDLobbyGameMode::GenericPlayerInitialization(AController* C)
{
	Super::GenericPlayerInitialization(C);
	
	ADDLobbyPlayerController* LobbyPC = Cast<ADDLobbyPlayerController>(C);
	if (!IsValid(LobbyPC)) return;

	if (!LobbyUIConfig) return;

	// 초기 UI 초기화 
	// UIConfig 전달 → 클라이언트 UIManager 초기화
	LobbyPC->Client_SetUIConfig(LobbyUIConfig);

	// 닉네임 입력창 팝업
	LobbyPC->Client_OpenPopUp(DDGameplayTags::Lobby_UI_NicknamePopup);
}

bool ADDLobbyGameMode::TryRegisterPlayerNickname(
	ADDLobbyPlayerController* Requester, 
	const FName& Nickname,
	FString& ErrorMessage)
{
	// 1. 유효성 및 중복 체크
	if (!IsValid(GetLobbyGameState()))
	{
		ErrorMessage = TEXT("시스템 오류가 발생했습니다. : GameState가 유효하지않음."); 
		return false; 
	}
	
	if (!IsValid(Requester))
	{
		ErrorMessage = TEXT("유효하지 않은 컨트롤러 입니다."); 
		return false; 
	}
		// 닉네임 길이, 중복 여부 체크 
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
	
	// 2. 닉네임 변경 
	PS->SetPlayerName(Nickname.ToString());
	PS->PlayerGameData.PlayerDisplayName = Nickname;
	
	// 3. 캐릭터 색상 설정  << 이것도 게임모드에서 할건 아닌듯? 
	FLinearColor NewRandomColor = GetRandomAvailableColor();
	PS->SetPlayerColor(NewRandomColor);
	
	// 4. 일단 참가자로 요청 (초과시 관전자로 전환됨) 
	AssignPlayerRole(Requester, ELobbyPlayerRole::Participant);
	Requester->Client_JoinLobby();
	
	return true; 
}

void ADDLobbyGameMode::RequestPlayerReady(ADDLobbyPlayerController* Requester, bool bReady)
{
	if(!GetLobbyGameState()) return;
	
	FLobbyParticipantInfo* Found = FindParticipant(Requester);
	if (!Found) return; 
	
	if (bReady)
	{
		Found->bIsReady = true; 
		CachedGameState->ReadyCount++;
		
		if (CachedGameState->ReadyCount == GetParticipantCount() &&
			CachedGameState->ReadyCount >= CachedGameState->RequiredPlayerCount)
		{
			StartBoardGameCountdown(StartCountdownSeconds); 
		}
	}
	else
	{
		Found->bIsReady = false; 
		CachedGameState->ReadyCount = FMath::Max(0, CachedGameState->ReadyCount-1);
		
		// 시작 중이라면 타이머 초기화 
		GetWorld()->GetTimerManager().ClearTimer(StartTimerHandle);
		CachedGameState->CountDown = -1; 
	}
}

void ADDLobbyGameMode::AssignPlayerRole(ADDLobbyPlayerController* Requester, ELobbyPlayerRole RequestedRole)
{
	ELobbyPlayerRole FinalRole = RequestedRole;
	
	if (FinalRole == ELobbyPlayerRole::Participant) 
	{
		int32 CurrentParticipantCount = GetParticipantCount(); 
	
		if (CurrentParticipantCount >= CachedGameState->MaxPlayerCount)
		{
			FinalRole = ELobbyPlayerRole::Spectator;
			LOG_CJH(Log, TEXT("최대 인원 초과 → 관전자로 변경"));
		}
	}
	
	// 기존에 있던 플레이어 인지 확인 
	FLobbyParticipantInfo* Found = Participants.FindByPredicate(
		[Requester](const FLobbyParticipantInfo& P){ return P.Controller == Requester; }
	);

	if (Found)
	{
		// 역할만 변경 
		Found->Role = FinalRole;
	}
	else
	{
		// 신규 플레이어 추가 
		Participants.Add({ Requester, false, FinalRole });
	}
	
	ADDBasePlayerState* PS = Requester->GetPlayerState<ADDBasePlayerState>();
	if (!IsValid(PS)) return;
	
	if (FinalRole == ELobbyPlayerRole::Participant)
	{
		PS->bIsParticipant = true;
		CachedGameState->PlayerCount++;
	}
	else
	{
		PS->bIsParticipant = false;
		SetPlayerAsSpectator(Requester);
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

void ADDLobbyGameMode::StartBoardGameCountdown(float InSeconds)
{
	GetLobbyGameState()->CountDown = FMath::FloorToInt(InSeconds);
	LOG_CJH(Log, TEXT("곧 게임을 시작합니다."));
	
	// 1초마다 대기실 인원 체크 및 카운트다운을 수행하는 타이머 실행
	GetWorld()->GetTimerManager().SetTimer(
		StartTimerHandle, 
		this, 
		&ThisClass::OnCountdownTick, 
		1.f, 
		true
	);
}

void ADDLobbyGameMode::OnCountdownTick()
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
	if (!IsValid(GetLobbyGameState()))
	{
		ErrorMessage = TEXT("시스템 오류가 발생했습니다. 오류 : GameState가 유효하지 않음.");
		return false; 
	}
	
	if (GetParticipantCount() < CachedGameState->RequiredPlayerCount)
	{
		ErrorMessage = FString::Printf(
			TEXT("시작할려면 최소 %d 명 이상 되어야 합니다."),
			CachedGameState->RequiredPlayerCount
		);
		
		return false; 
	}
	
	if (CachedGameState->ReadyCount < CachedGameState->RequiredPlayerCount)
	{
		ErrorMessage = FString::Printf(
		TEXT("모든 플레이어가 준비해야 합니다. (준비 %d / %d명)"),
			CachedGameState->ReadyCount,
			CachedGameState->PlayerCount
		);
		
		return false;
	}
	
	return true; 
}

void ADDLobbyGameMode::TryStartBoardGame()
{
	if (!IsValid(GetLobbyGameState())) return;
	
	FString ErrorMessage; 
	if (!CanStartBoardGame(ErrorMessage))
	{
		LOG_KMS(Warning, TEXT("%s"), *ErrorMessage);
		return; 
	}
	
	LOG_KMS(Log, TEXT("게임이 시작됩니다.")); 
	
	
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

int32 ADDLobbyGameMode::GetParticipantCount() const
{
	int32 CurrentParticipantCount = Algo::CountIf(
		Participants,
		[](const FLobbyParticipantInfo& P)
		{
			return P.Role == ELobbyPlayerRole::Participant;
		}
	);
	
	return CurrentParticipantCount;
}

FLobbyParticipantInfo* ADDLobbyGameMode::FindParticipant(ADDLobbyPlayerController* Requester)
{
	FLobbyParticipantInfo* Found = 
		Participants.FindByPredicate([Requester](const FLobbyParticipantInfo& P)
		{
			return P.Controller == Requester;
		});
	
	return Found; 
}
