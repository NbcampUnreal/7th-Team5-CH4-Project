// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardGame/DDTestBoardGameMode.h"

#include "EngineUtils.h"
#include "Base/Player/DDBasePlayerState.h"
#include "BoardGame/DDTileManager.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "Common/DDLogManager.h"
#include "GameFramework/GameStateBase.h"

void ADDTestBoardGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;
	
}

void ADDTestBoardGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	LOG_CYS(Warning, TEXT("[TestGM] PostLogin"));

	// TileManager 찾기
	ADDTileManager* TM = nullptr;
	for (TActorIterator<ADDTileManager> It(GetWorld()); It; ++It)
	{
		TM = *It;
		break;
	}

	if (!TM) return;

	// 타일 초기화 (한 번만)
	// if (!TM->bIsReady)
	// {
		TM->InitializeTiles();
	// }

	// PlayerState 초기화
	ADDBasePlayerState* PS = NewPlayer->GetPlayerState<ADDBasePlayerState>();
	if (PS)
	{
		PS->InitTile();
	}

	// Pawn 아직 없을 수도 있음 → RestartPlayer 이후라서 보통 있음
	ADDBoardGameCharacter* Char = Cast<ADDBoardGameCharacter>(NewPlayer->GetPawn());
	if (Char)
	{
		Char->InitLocation();
	}
}