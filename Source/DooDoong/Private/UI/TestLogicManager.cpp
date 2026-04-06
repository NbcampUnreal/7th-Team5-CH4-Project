#include "UI/TestLogicManager.h"

void UTestLogicManager::InitTestPlayers()
{
    Players.Empty();

    FTestPlayerData P1; P1.Name = "A"; P1.Gold = 100; P1.Health = 80;
    FTestPlayerData P2; P2.Name = "B"; P2.Gold = 50;  P2.Health = 100;
    FTestPlayerData P3; P3.Name = "C"; P3.Gold = 200; P3.Health = 60;
    FTestPlayerData P4; P4.Name = "D"; P4.Gold = 10;  P4.Health = 90;

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