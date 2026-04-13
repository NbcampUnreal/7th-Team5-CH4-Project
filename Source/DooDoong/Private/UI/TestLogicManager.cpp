#include "UI/TestLogicManager.h"

void UTestLogicManager::InitTestPlayers()
{
    Players.Empty();

    FTestPlayerData P1;
    P1.Name = "cuty";
    P1.Gold = 100;
    P1.Health = 80;
    P1.ProfileImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/DooDoong/05_UI/HUD/UI_Assets/PlayerProfile/p1.p1"));

    FTestPlayerData P2;
    P2.Name = "duck";
    P2.Gold = 50;
    P2.Health = 100;
    P2.ProfileImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/DooDoong/05_UI/HUD/UI_Assets/PlayerProfile/p2.p2"));

    FTestPlayerData P3;
    P3.Name = "baby";
    P3.Gold = 200;
    P3.Health = 60;
    P3.ProfileImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/DooDoong/05_UI/HUD/UI_Assets/PlayerProfile/p3.p3"));

    FTestPlayerData P4;
    P4.Name = "mommy";
    P4.Gold = 10;
    P4.Health = 90;
    P4.ProfileImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/DooDoong/05_UI/HUD/UI_Assets/PlayerProfile/p4.p4"));

    Players.Add(P1);
    Players.Add(P2);
    Players.Add(P3);
    Players.Add(P4);
}

void UTestLogicManager::GiveGold(int32 PlayerIndex, int32 Amount)
{
    if (Players.IsValidIndex(PlayerIndex))
    {
        Players[PlayerIndex].Gold += Amount;
    }
}

void UTestLogicManager::GiveHealth(int32 PlayerIndex, int32 Amount)
{
    if (Players.IsValidIndex(PlayerIndex))
    {
        Players[PlayerIndex].Health += Amount;
    }
}

TArray<FTestPlayerData> UTestLogicManager::GetSortedPlayers()
{
    TArray<FTestPlayerData> Sorted = Players;

    Sorted.Sort([](const FTestPlayerData& A, const FTestPlayerData& B)
        {
            if (A.Gold != B.Gold)
                return A.Gold > B.Gold;

            return A.Health > B.Health;
        });

    return Sorted;
}


void UTestLogicManager::SetHealth(int32 PlayerIndex, int32 NewValue)
{
    if (Players.IsValidIndex(PlayerIndex))
    {
        Players[PlayerIndex].Health = NewValue;

        // ?? ???? ??? UI ?? ??? ??
    }
}


