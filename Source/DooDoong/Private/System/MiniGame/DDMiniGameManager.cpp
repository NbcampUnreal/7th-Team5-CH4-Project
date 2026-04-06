#include "System/MiniGame/DDMiniGameManager.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Base/MiniGame/DDMiniGameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "System/MiniGame/DDMiniGameDefinition.h"

void UDDMiniGameManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 시작지점에서 미니게임 정의 목록을 한 번 런타임 캐싱
	CacheAvailableDefinitions();
	// 마지막 진행 흔적을 초기화
	ClearLastCommittedResult();
	ClearActiveSession();
}

void UDDMiniGameManager::Deinitialize()
{
	// 종료 시 런타임 캐시 정리
	CachedDefinitions.Reset();
	// 마지막 진행 정보들을 정리
	ClearLastCommittedResult();
	ClearActiveSession();
	
	Super::Deinitialize();
}

bool UDDMiniGameManager::RequestStartMiniGame(FName MiniGameId, const TArray<APlayerState*>& Players)
{
	if (CurrentState != DDGameplayTags::State_MiniGame_Idle || MiniGameId.IsNone())
	{
		return false;
	}
	
	UDDMiniGameDefinition* Definition = FindMiniGameDefinition(MiniGameId);
	UWorld* World = GetWorld();
	
	// World->GetNetMode() == NM_Client 인 경우에 false를 return 하므로 클라이언트에서 사용할 수 없음. 반드시 서버에서 사용
	if (Definition == nullptr || Definition->GameModeClass == nullptr || World == nullptr || World->GetNetMode() == NM_Client)
	{
		return false;
	}

	// 선택한 정의와 참가자 목록을 실제 실행 데이터로 변환
	if (!BuildSetupFromDefinition(Definition, Players))
	{
		return false;
	}

	ActiveDefinition = Definition;
	// UI가 어떤 미니게임이 선택되었는지 바로 알 수 있도록 선택 시점에 먼저 알림.
	OnMiniGameSelected.Broadcast(ActiveDefinition);
	// 미니게임 종료 후 되돌아올 월드 이름을 저장해둠.
	ReturnMapPackageName = World->GetPackage()->GetName();

	// 상태를 준비 상태로 갱신
	SetCurrentState(DDGameplayTags::State_MiniGame_Preparing);
	OnMiniGamePreparing.Broadcast(ActiveSetup);

	// 상태를 미니게임으로 이동하는 중으로 갱신 & 미니게임 맵으로 이동
	SetCurrentState(DDGameplayTags::State_MiniGame_Traveling);
	return TravelToMap(ActiveSetup.MiniGameMap);
}

bool UDDMiniGameManager::RequestStartRandomMiniGame(const TArray<APlayerState*>& Players)
{
	if (CachedDefinitions.Num() == 0)
	{
		CacheAvailableDefinitions();
	}

	// 미니게임 선택지 후보 배열
	TArray<FName> CandidateMiniGameIds;
	CandidateMiniGameIds.Reserve(CachedDefinitions.Num());

	for (const TPair<FName, TObjectPtr<UDDMiniGameDefinition>>& Entry : CachedDefinitions)
	{
		if (Entry.Key.IsNone() || Entry.Value == nullptr || Entry.Value->GameModeClass == nullptr)
		{
			continue;
		}

		CandidateMiniGameIds.Add(Entry.Key);
	}

	if (CandidateMiniGameIds.Num() == 0)
	{
		return false;
	}

	// 캐시에 있는 미니게임 정의 중 하나를 랜덤으로 골라 기존 시작 로직에 연결
	const int32 RandomIndex = FMath::RandRange(0, CandidateMiniGameIds.Num() - 1);
	return RequestStartMiniGame(CandidateMiniGameIds[RandomIndex], Players);
}

bool UDDMiniGameManager::RequestFinishMiniGame()
{
	if (CurrentState != DDGameplayTags::State_MiniGame_Playing)
	{
		return false;
	}
	
	// 실제 게임 종료 판정은 현재 미니게임의 GameMode에 위임
	if (UWorld* World = GetWorld())
	{
		if (ADDMiniGameModeBase* MiniGameMode = Cast<ADDMiniGameModeBase>(World->GetAuthGameMode()))
		{
			MiniGameMode->FinishMiniGame();
			return true;
		}
	}

	// 미니게임 종료 요청
	StopActiveMiniGame(DDGameplayTags::FinishReason_MiniGame_Completed);
	return true;
}

void UDDMiniGameManager::StopActiveMiniGame(FGameplayTag Reason)
{
	if (CurrentState == DDGameplayTags::State_MiniGame_Idle)
	{
		return;
	}

	if (CurrentState == DDGameplayTags::State_MiniGame_Playing)
	{
		if (UWorld* World = GetWorld())
		{
			if (ADDMiniGameModeBase* MiniGameMode = Cast<ADDMiniGameModeBase>(World->GetAuthGameMode()))
			{
				MiniGameMode->StopMiniGame(Reason);
				return;
			}
		}
	}

	FMiniGameResult Result;
	Result.MiniGameId = ActiveSetup.MiniGameID;
	Result.FinishReason = Reason;
	// 혹시나 플레이 중인 GameMode가 없는 경우에 최소한의 결과만 만들어 줌.
	CommitMiniGameResult(Result);
}

void UDDMiniGameManager::CommitMiniGameResult(const FMiniGameResult& Result)
{
	if (CurrentState == DDGameplayTags::State_MiniGame_Idle)
	{
		return;
	}

	SetCurrentState(DDGameplayTags::State_MiniGame_Finishing);
	OnMiniGameFinished.Broadcast(Result);

	LastCommittedResult = Result;
	bHasLastCommittedResult = true;

	SetCurrentState(DDGameplayTags::State_MiniGame_Completed);
	OnMiniGameResultCommitted.Broadcast(Result);

	const FString SavedReturnMapPackageName = ReturnMapPackageName;
	ClearActiveSession();

	if (!SavedReturnMapPackageName.IsEmpty())
	{
		SetCurrentState(DDGameplayTags::State_MiniGame_Returning);

		if (UWorld* World = GetWorld())
		{
			// 미니게임이 끝나면 ServerTravel로 참가자 전원을 원래 맵으로 복귀시킴
			// TODO 보드판이 준비되면 단순한 복귀가 아니라 보드칸의 정보를 확인해서 원래 플레이어가 서 있던 칸으로 복귀시켜야함.
			World->ServerTravel(SavedReturnMapPackageName);
		}
	}

	SetCurrentState(DDGameplayTags::State_MiniGame_Idle);
}

void UDDMiniGameManager::NotifyMiniGameStarted()
{
	// 맵 이동 후에만 Playing 상태로 진입할 수 있어야 하기 때문에 Preparing, Traveling 상태가 아니라면 return
	if (CurrentState != DDGameplayTags::State_MiniGame_Preparing && CurrentState != DDGameplayTags::State_MiniGame_Traveling)
	{
		return;
	}

	OnMiniGameStarted.Broadcast();
	SetCurrentState(DDGameplayTags::State_MiniGame_Playing);
}

void UDDMiniGameManager::ClearLastCommittedResult()
{
	// 이전 결과를 지워 다음 미니게임 실행과 구분할 필요가 있음
	LastCommittedResult = FMiniGameResult();
	bHasLastCommittedResult = false;
}

UDDMiniGameDefinition* UDDMiniGameManager::FindMiniGameDefinition(FName MiniGameId)
{
	if (MiniGameId.IsNone())
	{
		return nullptr;
	}
	
	// 우선 캐시된 정의에서 찾고
	if (TObjectPtr<UDDMiniGameDefinition>* CachedDefinition = CachedDefinitions.Find(MiniGameId))
	{
		return CachedDefinition->Get();
	}
	
	// 없으면 에셋 목록을 다시 스캔
	CacheAvailableDefinitions();

	if (TObjectPtr<UDDMiniGameDefinition>* CachedDefinition = CachedDefinitions.Find(MiniGameId))
	{
		return CachedDefinition->Get();
	}

	return nullptr;
}

bool UDDMiniGameManager::IsActiveMiniGameWorld(const UWorld* World) const
{
	if (World == nullptr || !ActiveSetup.MiniGameMap.ToSoftObjectPath().IsValid())
	{
		return false;
	}

	// 현재 월드가 선택된 미니게임 맵과 같은지 확인
	const FString ActiveMiniGamePackageName = FPackageName::ObjectPathToPackageName(ActiveSetup.MiniGameMap.ToSoftObjectPath().ToString());
	return !ActiveMiniGamePackageName.IsEmpty() && World->GetPackage()->GetName() == ActiveMiniGamePackageName;
}

void UDDMiniGameManager::SetCurrentState(FGameplayTag NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}
	
	CurrentState = NewState;
	// 상태가 바뀔 때만 브로드캐스트해서 UI나 로직이 중복 반응하지 않게 함.
	OnMiniGameStateChanged.Broadcast(CurrentState);
}

bool UDDMiniGameManager::BuildSetupFromDefinition(const UDDMiniGameDefinition* Definition,
	const TArray<APlayerState*>& Players)
{
	if (Definition == nullptr)
	{
		return false;
	}

	ActiveSetup = FMiniGameSetup();
	ActiveSetup.MiniGameID = Definition->MiniGameID;
	ActiveSetup.Genre = Definition->Genre;
	ActiveSetup.TimeLimitSeconds = Definition->DefaultTimeLimitSeconds;
	ActiveSetup.MiniGameTags = Definition->MiniGameTags;
	ActiveSetup.MiniGameMap = Definition->MiniGameMap;
	ActiveParticipants.Reset();

	for (int32 Index = 0; Index < Players.Num(); ++Index)
	{
		if (Players[Index] == nullptr)
		{
			continue;
		}

		FMiniGameParticipantInfo Participant;
		Participant.PlayerState = Players[Index];
		Participant.PlayerId = Players[Index]->GetPlayerId();
		Participant.SlotIndex = Index;
		Participant.Id = Index;
		Participant.bReady = true;
		
		// 참가자 정보는 맵 이동 후 GameMode와 GameState를 초기화할 때 재사용
		ActiveParticipants.Add(Participant);
	}

	// 최소/최대 인원 검증은 로비나 상위 게임 흐름에서 처리해 줄 것이니까
	// 실제 참가자가 한 명 이상 구성되었는지만 확인
	return ActiveParticipants.Num() > 0;
}

void UDDMiniGameManager::ClearActiveSession()
{
	// 현재 진행 중인 미니게임과 관련된 런타임 데이터만 Clear
	ActiveDefinition = nullptr;
	ActiveParticipants.Reset();
	ActiveSetup = FMiniGameSetup();
	ReturnMapPackageName.Reset();
}

bool UDDMiniGameManager::TravelToMap(const TSoftObjectPtr<UWorld>& MapAsset)
{
	UWorld* World = GetWorld();
	const FSoftObjectPath MapPath = MapAsset.ToSoftObjectPath();
	// 잘못된 맵이거나 클라이언트의 월드라면 이동을 중단하고 상태를 초기화
	if (World == nullptr || World->GetNetMode() == NM_Client || !MapPath.IsValid())
	{
		ClearActiveSession();
		SetCurrentState(DDGameplayTags::State_MiniGame_Idle);
		return false;
	}

	const FString TravelURL = FPackageName::ObjectPathToPackageName(MapPath.ToString());
	if (TravelURL.IsEmpty())
	{
		ClearActiveSession();
		SetCurrentState(DDGameplayTags::State_MiniGame_Idle);
		return false;
	}

	// 미니게임 진입은 클라이언트가 아니라 서버의 맵 전환으로 처리
	World->ServerTravel(TravelURL);
	return true;
}

void UDDMiniGameManager::CacheAvailableDefinitions()
{
	CachedDefinitions.Reset();

	// AssetRegistry를 쓴 이유 : UMiniGameDefinition 데이터 에셋들을 프로젝트 전체에서 자동으로 찾아오기 위함
	// 데이터 에셋을 계속 추가하면서 미니게임을 수를 꽤 확장해나가야하는 구조상 자동으로 가져오는 것이 적합하다고 판단
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> DefinitionAssets;
	// 프로젝트에 존재하는 모든 미니게임 정의 에셋을 찾아서 저장함.
	AssetRegistryModule.Get().GetAssetsByClass(UDDMiniGameDefinition::StaticClass()->GetClassPathName(), DefinitionAssets, true);

	for (const FAssetData& AssetData : DefinitionAssets)
	{
		// AssetData는 메타데이터이므로 실제 정의 객체로 캐스팅해서 사용.
		UDDMiniGameDefinition* Definition = Cast<UDDMiniGameDefinition>(AssetData.GetAsset());
		if (Definition == nullptr || Definition->MiniGameID.IsNone())
		{
			continue;
		}

		CachedDefinitions.Add(Definition->MiniGameID, Definition);
	}
}
