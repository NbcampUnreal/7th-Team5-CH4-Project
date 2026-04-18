#include "BoardGame/Game/DDBoardGameState.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "BoardGame/Game/DDBoardGameMode.h"
#include "Common/DDLogManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "System/DDGameplayTags.h"

ADDBoardGameState::ADDBoardGameState()
{
	bReplicates = true;
}

void ADDBoardGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

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
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(
		GetWorld(),
		FName("IntroSequence"),
		FoundActors
	);

	if (FoundActors.Num() > 0)
	{
		LevelSequenceActor = Cast<ALevelSequenceActor>(FoundActors[0]);
		LOG_CYS(Warning,TEXT("[GS] 인트로 시퀀서 찾음"));
	}
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
				SequencePlayer->OnFinished.AddDynamic(this, &ADDBoardGameState::OnSequenceFinished);
			}
			SequencePlayer->Play();
		}
	}
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
