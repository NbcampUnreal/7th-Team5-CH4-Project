#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace DDGameplayTags
{
	// Native Input Tags 
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Native_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Native_Look);
	
	// Ability Input Tags 
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability_Jump); 
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability_RollDice);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability_Attack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability_Shot);
	
	// Character State Tags
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Character_Attacking); 
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Character_Death);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Character_Stun);
	
	// Event Tags 
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Character_Death);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Trace_Start); 
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Trace_End); 
	
	// Data Tags
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Health_Damage); 
	
	// Ability Tile Event Tags
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tile_Ability_Coin);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tile_Ability_Item);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tile_Ability_Move);
	
	// 미니게임 상태 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_MiniGame_Idle);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_MiniGame_Preparing);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_MiniGame_Traveling);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_MiniGame_Playing);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_MiniGame_Finishing);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_MiniGame_Returning);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_MiniGame_Completed);

	// 미니게임 장르 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Genre_MiniGame_None);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Genre_MiniGame_Competition);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Genre_MiniGame_Survival);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Genre_MiniGame_Race);
	
	// 미니게임 종료 조건 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(FinishReason_MiniGame_TimeOver);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(FinishReason_MiniGame_Completed);
	
	// 보드게임 로비 상태 태그 
  UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_BoardGame_LobbyWaiting);
  UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_BoardGame_LobbyStarting);

  // 보드게임 보드판 상태 태그
  UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_BoardGame_Init);
  UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_BoardGame_PlayerTurn);
  UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_BoardGame_RoundEnd);
 
  // 보드게임 종료 상태 태그 
  UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_BoardGame_Ending);
  UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_BoardGame_End);

  // 보드게임 턴 제어 상태 태그
  UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_BoardGame_TurnActive);
  UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_BoardGame_TurnWaiting);
	
	// 보드게임 턴 내부 세부 페이즈 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_TurnPhase_BeforeDice);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_TurnPhase_Moving);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_TurnPhase_Event);
}
