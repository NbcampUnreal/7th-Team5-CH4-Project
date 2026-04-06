#include "System/DDGameplayTags.h"


namespace DDGameplayTags
{
	// Native Input Tags 
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_Move, "Input.Native.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_Look, "Input.Native.Look");
	
	// Ability Input Tags 
	UE_DEFINE_GAMEPLAY_TAG(Input_Ability_Jump, "Input.Ability.Jump"); 
	UE_DEFINE_GAMEPLAY_TAG(Input_Ability_RollDice, "Input.Ability.RollDice");
	UE_DEFINE_GAMEPLAY_TAG(Input_Ability_Attack, "Input.Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability_Shot, "Input.Ability.Shot", "슈터 미니게임 전용 발사 어빌리티");
	
	// Character State Tags
	UE_DEFINE_GAMEPLAY_TAG(State_Character_Death, "State.Character.Death");
	UE_DEFINE_GAMEPLAY_TAG(State_Character_Stun, "State.Character.Stun");
	
	// Event Tags 
	UE_DEFINE_GAMEPLAY_TAG(Event_Trace_Start, "Event.Trace.Start");
	UE_DEFINE_GAMEPLAY_TAG(Event_Trace_End, "Event.Trace.End");
	
	// Data Tags
	UE_DEFINE_GAMEPLAY_TAG(Data_Health_Damage, "Data.Health.Damage");
	

	// Ability Tile Event Tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tile_Ability_Coin, "Tile.Ability.Coin", "코인 이벤트 타일 어빌리티");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tile_Ability_Item, "Tile.Ability.Item", "아이템 이벤트 타일 어빌리티");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tile_Ability_Move, "Tile.Ability.Move", "이동 이벤트 타일 어빌리티");

	// 미니게임 상태 태그
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_MiniGame_Idle, "State.MiniGame.Idle", "미니게임 세션이 없고 대기 중인 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_MiniGame_Preparing, "State.MiniGame.Preparing", "미니게임 시작을 준비하는 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_MiniGame_Traveling, "State.MiniGame.Traveling", "플레이어가 미니게임 맵으로 이동 중인 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_MiniGame_Playing, "State.MiniGame.Playing", "미니게임이 현재 진행 중인 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_MiniGame_Finishing, "State.MiniGame.Finishing", "미니게임 결과를 정리하고 종료 처리 중인 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_MiniGame_Returning, "State.MiniGame.Returning", "플레이어가 원래 맵으로 복귀 중인 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_MiniGame_Completed, "State.MiniGame.Completed", "미니게임이 완료된 상태");

	// 미니게임 장르 태그
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Genre_MiniGame_None, "Genre.MiniGame.None", "지정되지 않은 미니게임 장르");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Genre_MiniGame_Competition, "Genre.MiniGame.Competition", "경쟁 미니게임 장르");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Genre_MiniGame_Survival, "Genre.MiniGame.Survival", "생존 미니게임 장르");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Genre_MiniGame_Race, "Genre.MiniGame.Race", "레이스 미니게임 장르");

	// 미니게임 종료 조건 태그
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FinishReason_MiniGame_TimeOver, "FinishReason.MiniGame.TimeOver", "제한 시간이 종료되어 미니게임이 끝난 경우");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FinishReason_MiniGame_Completed, "FinishReason.MiniGame.Completed", "승리 조건이나 완료 조건을 충족해 미니게임이 끝난 경우");

	// 보드게임 로비 상태 태그 
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_LobbyWaiting, "State.BoardGame.LobbyWaiting", "로비 대기");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_LobbyStarting, "State.BoardGame.LobbyStarting", "로비 카운트다운");
    
    // 보드게임 보드판 상태 태그 
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_Init, "State.BoardGame.Init", "보드판 라운드 시작");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_PlayerTurn, "State.BoardGame.PlayerTurn", "플레이어 턴 진행");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_RoundEnd, "State.BoardGame.RoundEnd", "턴 종료 및 대기");
	
    // 보드게임 종료 상태 태그
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_Ending, "State.BoardGame.Ending", "최종 승자 판정");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_End, "State.BoardGame.End", "결과창 출력");

    // 보드게임 턴 제어 상태 태그
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_TurnActive, "State.BoardGame.TurnActive", "내 턴");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_TurnWaiting, "State.BoardGame.TurnWaiting", "남의 턴");
	
	// 보드게임 턴 내부 세부 페이즈 태그
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_TurnPhase_BeforeDice, "State.TurnPhase.BeforeDice", "주사위 굴리기 전 (아이템 사용 가능)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_TurnPhase_Moving, "State.TurnPhase.Moving", "보드판 이동 중 (모든 조작 불가)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_TurnPhase_Event, "State.TurnPhase.Event", "이동 완료 후 (상점 이용 및 트로피 획득 판정 중)");
}

