#include "Base/MiniGame/DDMiniGameStateBase.h"

#include "Base/Player/DDBasePlayerController.h"
#include "Base/Player/DDBasePlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "InputMappingContext.h"
#include "Net/UnrealNetwork.h"
#include "System/DDSoundManager.h"
#include "System/MiniGame/DDMiniGameDefinition.h"
#include "System/MiniGame/DDMiniGameManager.h"

ADDMiniGameStateBase::ADDMiniGameStateBase()
{
	bReplicates = true;
}

void ADDMiniGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 진행 상태와 점수판은 모든 클라이언트가 동일하게 봐야하기 때문에 복제
	DOREPLIFETIME(ADDMiniGameStateBase, MiniGameSetup);
	DOREPLIFETIME(ADDMiniGameStateBase, bMiniGameSetupReady);
	DOREPLIFETIME(ADDMiniGameStateBase, CurrentState);
	DOREPLIFETIME(ADDMiniGameStateBase, RemainingTimeSeconds);
	DOREPLIFETIME(ADDMiniGameStateBase, Participants);
	DOREPLIFETIME(ADDMiniGameStateBase, ScoreBoard);
	DOREPLIFETIME(ADDMiniGameStateBase, ReadyPlayerCount);
	DOREPLIFETIME(ADDMiniGameStateBase, TotalParticipantCount);
	DOREPLIFETIME(ADDMiniGameStateBase, ReadyEntries);
}

void ADDMiniGameStateBase::SetMiniGameState(FGameplayTag NewState)
{
	// 현재 진행 단계를 갱신할 때 사용
	if (CurrentState == NewState)
	{
		return;
	}
	
	CurrentState = NewState;
	OnMiniGameStateTagChanged.Broadcast(CurrentState); 
	
	if (CurrentState == DDGameplayTags::State_MiniGame_Preparing ||
		CurrentState == DDGameplayTags::State_MiniGame_Playing)
	{
		ApplyMiniGameInputLocally();
	}
	if (CurrentState == DDGameplayTags::State_MiniGame_Playing)
	{
		PlayMiniGameBGM();
	}
	if (CurrentState == DDGameplayTags::State_MiniGame_Completed)
	{
		PlayFinishWhistle();
	}
}

void ADDMiniGameStateBase::SetMiniGameSetup(const FMiniGameSetup& Setup)
{
	MiniGameSetup = Setup;
	bMiniGameSetupReady = false;
	bHasBroadcastMiniGameSetup = false;
	bHasAppliedMiniGameInput = false;
}

void ADDMiniGameStateBase::NotifyMiniGameSetupReady()
{
	bMiniGameSetupReady = true;
	BroadcastMiniGameSetupChanged();
	
	if (CurrentState == DDGameplayTags::State_MiniGame_Playing)
	{
		PlayMiniGameBGM();
	}
	if (CurrentState == DDGameplayTags::State_MiniGame_Preparing ||
		CurrentState == DDGameplayTags::State_MiniGame_Playing)
	{
		ApplyMiniGameInputLocally();
	}
}

void ADDMiniGameStateBase::SetRemainingTimeSeconds(float NewRemainingTimeSeconds)
{
	// 게임의 남은 시간을 갱신 및 저장 (UI에서 사용하거나 할 수 있도록)
	RemainingTimeSeconds = NewRemainingTimeSeconds;
}

void ADDMiniGameStateBase::SetParticipants(const TArray<FMiniGameParticipantInfo>& InParticipants)
{
	// 현재 미니게임에 참여 중인 플레이어 목록을 갱신
	Participants = InParticipants;
}

void ADDMiniGameStateBase::SetScoreBoard(const TArray<FMiniGameScoreEntry>& InScoreBoard)
{
	// 룰셋에서 정리한 점수판을 통째로 반영할 때 사용할 수 있음
	ScoreBoard = InScoreBoard;
	// 브로드캐스트
	BroadcastScoreBoardChanged();
}

void ADDMiniGameStateBase::SetReadyPlayerCount(int32 NewReadyPlayerCount)
{
	ReadyPlayerCount = NewReadyPlayerCount;
	BroadcastReadyStateChanged();
}

void ADDMiniGameStateBase::SetTotalParticipantCount(int32 NewTotalParticipantCount)
{
	TotalParticipantCount = NewTotalParticipantCount;
	BroadcastReadyStateChanged();
}

void ADDMiniGameStateBase::SetReadyEntries(const TArray<FMiniGameReadyEntry>& InReadyEntries)
{
	ReadyEntries = InReadyEntries;
	BroadcastReadyEntriesChanged();
}

void ADDMiniGameStateBase::AddScore(APlayerState* PlayerState, int32 DeltaScore)
{
	if (PlayerState == nullptr)
	{
		return;
	}

	FName DisplayName = NAME_None;
	if (const ADDBasePlayerState* DDPlayerState = Cast<ADDBasePlayerState>(PlayerState))
	{
		DisplayName = DDPlayerState->PlayerGameData.PlayerDisplayName;
	}

	const int32 PlayerId = PlayerState->GetPlayerId();

	// 플레이어의 기존 점수 기록이 이미 ScoreBoard에 있는지 확인하는 작업.
	FMiniGameScoreEntry* ExistingEntry = ScoreBoard.FindByPredicate([PlayerId](const FMiniGameScoreEntry& Entry)
	{
		return Entry.PlayerId == PlayerId;
	});

	// 만약 스코어 보드에 없다면, 즉 플레이어가 처음으로 득점했다면 점수판 엔트리를 새로 만들어서 추가
	if (ExistingEntry == nullptr)
	{
		FMiniGameScoreEntry NewEntry;
		NewEntry.PlayerId = PlayerId;
		NewEntry.DisplayName = DisplayName;
		NewEntry.Score = DeltaScore;
		ScoreBoard.Add(NewEntry);
		return;
	}
	
	// 이미 점수를 획득했던 플레이어는 해당하는 점수판에 점수를 계속 누적하는 방식
	ExistingEntry->PlayerId = PlayerId;
	ExistingEntry->DisplayName = DisplayName;
	ExistingEntry->Score += DeltaScore;
}

int32 ADDMiniGameStateBase::GetScore(APlayerState* PlayerState) const
{
	if (PlayerState == nullptr)
	{
		return 0;
	}
	
	// 플레이어의 기존 점수 기록이 이미 ScoreBoard에 있는지 확인하는 작업.
	const int32 PlayerId = PlayerState->GetPlayerId();
	const FMiniGameScoreEntry* ExistingEntry = ScoreBoard.FindByPredicate([PlayerId](const FMiniGameScoreEntry& Entry)
	{
		return Entry.PlayerId == PlayerId;
	});

	// 플레이어의 기록이 있다면 return, 없다면 0을 return
	return ExistingEntry != nullptr ? ExistingEntry->Score : 0;
}

void ADDMiniGameStateBase::OnRep_MiniGameSetup()
{
	BroadcastMiniGameSetupChanged();
}

void ADDMiniGameStateBase::OnRep_MiniGameSetupReady()
{
	BroadcastMiniGameSetupChanged();
	
	if (CurrentState == DDGameplayTags::State_MiniGame_Playing)
	{
		PlayMiniGameBGM();
	}
	if (CurrentState == DDGameplayTags::State_MiniGame_Preparing ||
		CurrentState == DDGameplayTags::State_MiniGame_Playing)
	{
		ApplyMiniGameInputLocally();
	}
}

void ADDMiniGameStateBase::OnRep_CurrentState()
{
	if (CurrentState == DDGameplayTags::State_MiniGame_Preparing ||
		CurrentState == DDGameplayTags::State_MiniGame_Playing)
	{
		ApplyMiniGameInputLocally();
	}
	if (CurrentState == DDGameplayTags::State_MiniGame_Playing)
	{
		PlayMiniGameBGM();
	}
	else if (CurrentState == DDGameplayTags::State_MiniGame_Completed)
	{
		PlayFinishWhistle();
	}
}

void ADDMiniGameStateBase::OnRep_ReadyPlayerCount()
{
	BroadcastReadyStateChanged();
}

void ADDMiniGameStateBase::OnRep_TotalParticipantCount()
{
	BroadcastReadyStateChanged();
}

void ADDMiniGameStateBase::OnRep_ReadyEntries()
{
	BroadcastReadyEntriesChanged();
}

void ADDMiniGameStateBase::OnRep_ScoreBoard()
{
	BroadcastScoreBoardChanged();
}

void ADDMiniGameStateBase::OnRep_RemainingTimeSeconds()
{
	OnRemainingTimeChanged.Broadcast(RemainingTimeSeconds);
}

void ADDMiniGameStateBase::BroadcastMiniGameSetupChanged()
{
	if (!bMiniGameSetupReady || bHasBroadcastMiniGameSetup || MiniGameSetup.MiniGameID.IsNone())
	{
		return;
	}

	bHasBroadcastMiniGameSetup = true;
	OnMiniGameSetupChanged.Broadcast(MiniGameSetup);
}

void ADDMiniGameStateBase::BroadcastScoreBoardChanged()
{
	OnMiniGameScoreBoardChanged.Broadcast();
}

void ADDMiniGameStateBase::BroadcastReadyStateChanged()
{
	OnMiniGameReadyStateChanged.Broadcast(ReadyPlayerCount, TotalParticipantCount);
}

void ADDMiniGameStateBase::BroadcastReadyEntriesChanged()
{
	OnMiniGameReadyEntriesChanged.Broadcast(ReadyEntries);
}

void ADDMiniGameStateBase::PlayMiniGameBGM()
{
	if (GetNetMode() == NM_DedicatedServer || MiniGameSetup.BGM.IsNone())
	{
		return;
	}

	if (UDDSoundManager* SoundManager = UDDSoundManager::Get(this))
	{
		SoundManager->PlayBGM(MiniGameSetup.BGM, 0.5f);
	}
}

void ADDMiniGameStateBase::PlayFinishWhistle()
{
	if (GetNetMode() == NM_DedicatedServer || MiniGameSetup.BGM.IsNone())
	{
		return;
	}

	if (UDDSoundManager* SoundManager = UDDSoundManager::Get(this))
	{
		SoundManager->StopBGM(1);
		SoundManager->PlaySound2D("SFX_FinishWhistle");
	}
}

void ADDMiniGameStateBase::ApplyMiniGameInputLocally()
{
	if (bHasAppliedMiniGameInput || GetNetMode() == NM_DedicatedServer || MiniGameSetup.MiniGameID.IsNone())
	{
		return;
	}

	UWorld* World = GetWorld();
	ADDBasePlayerController* PlayerController = World != nullptr
		                                           ? Cast<ADDBasePlayerController>(World->GetFirstPlayerController())
		                                           : nullptr;
	if (PlayerController == nullptr || GetGameInstance() == nullptr)
	{
		return;
	}

	UDDMiniGameManager* MiniGameManager = GetGameInstance()->GetSubsystem<UDDMiniGameManager>();
	const UDDMiniGameDefinition* Definition = MiniGameManager != nullptr
		                                          ? MiniGameManager->FindMiniGameDefinition(MiniGameSetup.MiniGameID)
		                                          : nullptr;
	if (Definition == nullptr || Definition->MappingContextClass.IsNull())
	{
		return;
	}

	UInputMappingContext* MappingContext = Definition->MappingContextClass.LoadSynchronous();
	if (MappingContext == nullptr)
	{
		return;
	}

	PlayerController->SetInputMappingContext(MappingContext);
	bHasAppliedMiniGameInput = true;
}
