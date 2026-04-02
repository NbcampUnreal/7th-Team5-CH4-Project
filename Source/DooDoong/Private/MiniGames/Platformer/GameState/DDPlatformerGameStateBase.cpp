// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGames/Platformer/GameState/DDPlatformerGameStateBase.h"

ADDPlatformerGameStateBase::ADDPlatformerGameStateBase()
	: WaitingTime(10.f)
{
	
}

void ADDPlatformerGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	
	//@TODO_Minjae : 게임시작하면 플레이어 컨트롤러 정보 받아와서 대기시간동안 조작못하게 멈추기
}
