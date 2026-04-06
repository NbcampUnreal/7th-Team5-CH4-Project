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
	
	// Event Tags 
	UE_DEFINE_GAMEPLAY_TAG(Event_Trace_Start, "Event.Trace.Start");
	UE_DEFINE_GAMEPLAY_TAG(Event_Trace_End, "Event.Trace.End");
	
	// Data Tags
	UE_DEFINE_GAMEPLAY_TAG(Data_Health_Damage, "Data.Health.Damage");
	
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
}
