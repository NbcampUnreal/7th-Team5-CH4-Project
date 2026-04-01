#include "System/DDGameplayTags.h"


namespace DDGameplayTags
{
	// Native Input Tags 
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_Move, "Input.Native.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_Look, "Input.Native.Look");
	
	// Ability Input Tags 
	UE_DEFINE_GAMEPLAY_TAG(Input_Ability_Jump, "Input.Ability.Jump"); 

}

namespace DDMiniGameplayTags
{
	// 미니게임 상태 태그
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Idle, "MiniGame.State.Idle", "미니게임 세션이 없고 대기 중인 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Preparing, "MiniGame.State.Preparing", "미니게임 시작을 준비하는 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Traveling, "MiniGame.State.Traveling", "플레이어가 미니게임 맵으로 이동 중인 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Playing, "MiniGame.State.Playing", "미니게임이 현재 진행 중인 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Finishing, "MiniGame.State.Finishing", "미니게임 결과를 정리하고 종료 처리 중인 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Returning, "MiniGame.State.Returning", "플레이어가 원래 맵으로 복귀 중인 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Completed, "MiniGame.State.Completed", "미니게임이 완료된 상태");

	// 미니게임 장르 태그
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Genre_None, "MiniGame.Genre.None", "지정되지 않은 미니게임 장르");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Genre_Competition, "MiniGame.Genre.Competition", "경쟁 미니게임 장르");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Genre_Survival, "MiniGame.Genre.Survival", "생존 미니게임 장르");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Genre_Race, "MiniGame.Genre.Race", "레이스 미니게임 장르");

	// 미니게임 종료 조건 태그
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FinishReason_TimeOver, "MiniGame.FinishReason.TimeOver", "제한 시간이 종료되어 미니게임이 끝난 경우");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FinishReason_Completed, "MiniGame.FinishReason.Completed", "승리 조건이나 완료 조건을 충족해 미니게임이 끝난 경우");
}
