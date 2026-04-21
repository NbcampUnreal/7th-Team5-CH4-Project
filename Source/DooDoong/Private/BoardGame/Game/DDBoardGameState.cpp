#include "BoardGame/Game/DDBoardGameState.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "AbilitySystem/Attributes/DDPointSet.h"
#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/DDTileManager.h"
#include "BoardGame/Game/DDBoardGameMode.h"
#include "Common/DDLogManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "System/DDGameplayTags.h"
#include "System/DDSoundManager.h"

ADDBoardGameState::ADDBoardGameState()
{
	bReplicates = true;
}

void ADDBoardGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADDBoardGameState, MinPlayerCount);
	DOREPLIFETIME(ADDBoardGameState, MaxStateTimer);
	DOREPLIFETIME(ADDBoardGameState, MaxTrophy);
	DOREPLIFETIME(ADDBoardGameState, MaxRound);

	DOREPLIFETIME(ADDBoardGameState, BoardMatchStateTag);
	DOREPLIFETIME(ADDBoardGameState, TurnPlayerIndex);
	DOREPLIFETIME(ADDBoardGameState, StateTimer);
	DOREPLIFETIME(ADDBoardGameState, CurrentRound);
	DOREPLIFETIME(ADDBoardGameState, FinalRankings);
}

void ADDBoardGameState::BeginPlay()
{
	Super::BeginPlay();
	
	TileManager = Cast<ADDTileManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ADDTileManager::StaticClass())
	);
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(
		GetWorld(),
		FName("IntroSequence"),
		FoundActors
	);

	if (FoundActors.Num() > 0)
	{
		LevelSequenceActor = Cast<ALevelSequenceActor>(FoundActors[0]);
		LOG_CYS(Warning, TEXT("[GS] 인트로 시퀀서 찾음"));
	}
}

void ADDBoardGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	OnPlayerArrayChanged.Broadcast();
}

void ADDBoardGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	OnPlayerArrayChanged.Broadcast();
}

void ADDBoardGameState::OnSequenceFinished()
{
	if (!HasAuthority()) return; // 서버 체크

	ADDBoardGameMode* GM = GetWorld()->GetAuthGameMode<ADDBoardGameMode>();
	if (GM)
	{
		GM->SetMatchState(DDGameplayTags::State_BoardGame_PlayerTurn);
	}
}

void ADDBoardGameState::Multicast_PlaySequence_Implementation()
{
	if (LevelSequenceActor)
	{
		ULevelSequencePlayer* SequencePlayer = LevelSequenceActor->GetSequencePlayer();
		if (SequencePlayer)
		{
			// 서버에서만 바인딩 (중요)
			if (HasAuthority())
			{
				SequencePlayer->OnFinished.AddUniqueDynamic(this, &ADDBoardGameState::OnSequenceFinished);
			}
			SequencePlayer->OnFinished.AddUniqueDynamic(this, &ADDBoardGameState::PlayBoardBGMLocal);
			SequencePlayer->Play();
		}
	}
}

void ADDBoardGameState::Multicast_PlayBoardBGM_Implementation()
{
	PlayBoardBGMLocal();
}

void ADDBoardGameState::PlayBoardBGMLocal()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (UDDSoundManager* SoundMgr = UDDSoundManager::Get(this))
	{
		SoundMgr->PlayBGM("BGM_Board", 0.5f);
	}
}

TArray<ADDBasePlayerState*> ADDBoardGameState::UpdateAndGetPlayerRanks()
{
	TArray<ADDBasePlayerState*> SortedPlayers;
	for (APlayerState* PS : PlayerArray)
	{
		if (ADDBasePlayerState* BasePS = Cast<ADDBasePlayerState>(PS))
		{
			SortedPlayers.Add(BasePS);
		}
	}

	// 트로피 -> 코인 순으로 정렬
	SortedPlayers.Sort([](const ADDBasePlayerState& A, const ADDBasePlayerState& B)
	{
		if (A.GetPointSet() && B.GetPointSet())
		{
			float TrophyA = A.GetPointSet()->GetTrophy();
			float TrophyB = B.GetPointSet()->GetTrophy();

			if (TrophyA != TrophyB) return TrophyA > TrophyB;
			return A.GetPointSet()->GetCoin() > B.GetPointSet()->GetCoin();
		}
		return false;
	});

	// 정렬된 순서대로 각 PlayerState의 구조체 랭크 값 갱신
	for (int32 i = 0; i < SortedPlayers.Num(); ++i)
	{
		// OnRep_PlayerGameData를 확실하게 트리거하기 위해 복사본을 만들어 재할당합니다.
		FPlayerGameplayInfo TempInfo = SortedPlayers[i]->PlayerGameData;
		int32 NewRank = i + 1;

		if (TempInfo.CurrentRank != NewRank)
		{
			SortedPlayers[i]->SetCurrentRank(NewRank);
		}
	}

	return SortedPlayers;
}

void ADDBoardGameState::OnRep_StateTimer()
{
	OnStateTimerChanged.Broadcast(StateTimer);
}

void ADDBoardGameState::OnRep_CurrentRound()
{
	OnRoundChanged.Broadcast(CurrentRound);
}

void ADDBoardGameState::OnRep_FinalRankings()
{
	OnFinalRankingsUpdated.Broadcast(FinalRankings);
}
