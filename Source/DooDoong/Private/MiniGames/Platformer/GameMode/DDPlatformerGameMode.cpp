#include "MiniGames/Platformer/GameMode/DDPlatformerGameMode.h"
#include "MiniGames/Platformer/GameState/DDPlatformerGameState.h"
#include "Base/Player/DDBasePlayerController.h"
#include "Base/Player/DDBasePlayerState.h"
#include "Base/Character/DDBaseCharacter.h"
#include "UI/HUD/MiniGameReadyHUD.h"
#include "System/DDGameplayTags.h"
#include "Common/DDLogManager.h"

// 플레이어가 월드 이동 시 호출
// DDPlatformerGameMode.cpp

void ADDPlatformerGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
    Super::HandleSeamlessTravelPlayer(C);

    LOG_PMJ(Warning, TEXT("HandleSeamlessTravelPlayer called"));

    ADDBasePlayerController* BasePlayerController = Cast<ADDBasePlayerController>(C);
    if (!IsValid(BasePlayerController)) return;

    ADDBasePlayerState* BasePlayerState = C->GetPlayerState<ADDBasePlayerState>();
    if (!IsValid(BasePlayerState)) return;

    BasePlayerState->PlayerGameData.SlotIndex = PlayerIndex;

    FPlatformerPlayerData PlayerData;
    PlayerData.PlayerController = BasePlayerController;
    PlayerData.PlayerState = BasePlayerState;
    PlayerData.PlayerMaxDistance = 0;
    PlayerData.PlayerRank = -1;
    PlayerData.SavePointLocation = FVector::ZeroVector;
    PlayerData.PlayerSlotIndex = BasePlayerState->PlayerGameData.SlotIndex;
    PlayerData.PlayerDisplayName = BasePlayerState->PlayerGameData.PlayerDisplayName.ToString();
    PlayerData.PlayerColor = BasePlayerState->PlayerGameData.PlayerColor;

    LOG_PMJ(Warning, TEXT("Registered PlayerSlotIndex : %d"), PlayerData.PlayerSlotIndex);

    PlayerDatas.Add(PlayerData.PlayerSlotIndex, PlayerData);
    PlayerIndex++;

    // UI를 한 번만 띄우도록 플래그 체크
    if (!bReadyUIShown && PlayerDatas.Num() == MaxPlayer)
    {
        bReadyUIShown = true; // 한 번만 띄움

        for (TPair<int32, FPlatformerPlayerData>& Entry : PlayerDatas)
        {
            if (IsValid(Entry.Value.PlayerController.Get())) // TWeakObjectPtr 안전 체크
            {
                Entry.Value.PlayerController->Client_OpenReadyUI();
            }
        }
    }
}

// 게임 시작 시
void ADDPlatformerGameMode::BeginPlay()
{
    Super::BeginPlay();
    LOG_PMJ(Warning, TEXT("BeginPlayServer"));

    StartLocation = FVector::ZeroVector;

    // GameState 캐싱
    AGameStateBase* GameStateBase = GetWorld()->GetGameState();
    if (IsValid(GameStateBase))
    {
        PlatformerGameStateBase = Cast<ADDPlatformerGameState>(GameStateBase);
    }

    // 모든 플레이어 HUD에서 Ready UI 표시
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC) continue;

        AMiniGameReadyHUD* ReadyHUD = Cast<AMiniGameReadyHUD>(PC->GetHUD());
        if (ReadyHUD)
        {
            ReadyHUD->ShowReadyUI();
            LOG_PMJ(Warning, TEXT("ReadyUI shown for PlayerController %s"), *PC->GetName());
        }
    }
}

// 플레이어가 준비 완료 버튼 클릭 시 호출
void ADDPlatformerGameMode::SetPlayerReady(int32 PlayerSlotIndex)
{
    if (FPlatformerPlayerData* PlayerData = PlayerDatas.Find(PlayerSlotIndex))
    {
        PlayerData->bIsReady = true;
        LOG_PMJ(Warning, TEXT("PlayerSlotIndex %d is READY"), PlayerSlotIndex);
        CheckReadyPlayers();
    }
}

// 모든 플레이어 준비 여부 확인
void ADDPlatformerGameMode::CheckReadyPlayers()
{
    if (PlayerDatas.IsEmpty()) return;

    bool bIsAllReady = true;

    for (const TPair<int32, FPlatformerPlayerData>& Entry : PlayerDatas)
    {
        if (!Entry.Value.bIsReady)
        {
            bIsAllReady = false;
            break;
        }
    }

    if (bIsAllReady)
    {
        LOG_PMJ(Warning, TEXT("All players are READY"));



        WaitingTimerStart();
    }
    else
    {
        LOG_PMJ(Warning, TEXT("Not all players are ready yet"));
    }
}






void ADDPlatformerGameMode::StartMiniGame()
{
    Super::StartMiniGame();
    LOG_PMJ(Log, TEXT("Platformer StartMiniGame"));
    GameStart();
}

void ADDPlatformerGameMode::GameStart()
{
    LOG_PMJ(Log, TEXT("GameStart"));

    GetWorldTimerManager().ClearTimer(FinishedWaitingTimerHandle);

    GetWorldTimerManager().SetTimer(
        PlatformerPlayTimerHandle,
        this,
        &ADDPlatformerGameMode::GameEnd,
        PlatformerPlayTime,
        false
    );

    GetWorldTimerManager().SetTimer(
        DistanceTimerHandle,
        this,
        &ADDPlatformerGameMode::PlayGameTimer,
        1.f,
        true
    );
}

void ADDPlatformerGameMode::GameEnd()
{
    LOG_PMJ(Log, TEXT("GameEnd"));

    GetWorldTimerManager().ClearTimer(DistanceTimerHandle);

    for (const TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerDatas)
    {
        if (!EnteredPlayer.Value.bIsGoalIn)
        {
            PlayerNoGoalArrays.Add(EnteredPlayer);
        }
    }

    PlayerRanking();
}

void ADDPlatformerGameMode::PlayGameTimer()
{
    LOG_PMJ(Log, TEXT("PlayGameTimer"));

    for (TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerDatas)
    {
        FVector PlayerLocation = EnteredPlayer.Value.PlayerController->GetCharacter()->GetActorLocation();
        float CurrentPlayerDistance = FVector::Dist(StartLocation, PlayerLocation);

        if (EnteredPlayer.Value.PlayerMaxDistance < CurrentPlayerDistance)
        {
            EnteredPlayer.Value.PlayerMaxDistance = CurrentPlayerDistance;
        }
    }
}

void ADDPlatformerGameMode::CheckGoalPlayerCharacter(AActor* GoalActor)
{
    APawn* PlayerPawn = Cast<APawn>(GoalActor);
    if (!IsValid(PlayerPawn)) return;

    ADDBasePlayerState* BasePlayerState = PlayerPawn->GetPlayerState<ADDBasePlayerState>();
    if (!IsValid(BasePlayerState)) return;

    for (TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerDatas)
    {
        if (EnteredPlayer.Value.PlayerSlotIndex == BasePlayerState->PlayerGameData.SlotIndex)
        {
            if (!EnteredPlayer.Value.bIsGoalIn)
            {
                EnteredPlayer.Value.PlayerRank = Rank;
                LOG_PMJ(Warning, TEXT("PlayerRank : %d"), EnteredPlayer.Value.PlayerRank);
                Rank++;
                EnteredPlayer.Value.bIsGoalIn = true;
                PlayerRankingArrays.Add(EnteredPlayer);
            }
            else
            {
                LOG_PMJ(Warning, TEXT("Player already ranked %d"), EnteredPlayer.Value.PlayerRank);
            }
        }
    }
}

void ADDPlatformerGameMode::PlayerRanking()
{
    LOG_PMJ(Log, TEXT("PlayerRanking"));

    if (PlayerRankingArrays.Num() != MaxPlayer && !PlayerNoGoalArrays.IsEmpty())
    {
        PlayerNoGoalArrays.ValueSort([](const FPlatformerPlayerData& A, const FPlatformerPlayerData& B)
            {
                return A.PlayerMaxDistance > B.PlayerMaxDistance;
            });

        for (TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerNoGoalArrays)
        {
            EnteredPlayer.Value.PlayerRank = Rank++;
            PlayerRankingArrays.Add(EnteredPlayer);
        }
    }

    for (const TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerRankingArrays)
    {
        LOG_PMJ(Warning, TEXT("PlayerID : %d PlayerRank : %d"), EnteredPlayer.Value.PlayerSlotIndex, EnteredPlayer.Value.PlayerRank);
    }

    // TODO: AddScore 로직 필요
}

void ADDPlatformerGameMode::WaitingTimerStart()
{
    PlatformerGameStateBase->SetMiniGameState(DDGameplayTags::State_MiniGame_Preparing);

    for (const TPair<int32, FPlatformerPlayerData>& EnteredPlayer : PlayerDatas)
    {
        EnteredPlayer.Value.PlayerState->PlayerGameData.bPlayerIsDead = false;
    }

    LOG_PMJ(Log, TEXT("WaitingTimerStart"));

    GetWorldTimerManager().SetTimer(
        FinishedWaitingTimerHandle,
        this,
        &ADDPlatformerGameMode::GameStart,
        10.f,
        false
    );
}