#include "System/DDGameplayTags.h"


namespace DDGameplayTags
{
	// Native Input Tags 
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_Move, "Input.Native.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_Look, "Input.Native.Look");
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_Click, "Input.Native.Click");
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_Inventory, "Input.Native.Inventory");
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_ItemNextTarget, "Input.Native.ItemNextTarget");
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_ItemPreviousTarget, "Input.Native.ItemPreviousTarget");
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_ItemConfirm, "Input.Native.ItemConfirm");
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_ItemCancel, "Input.Native.ItemCancel");
	
	// Ability Input Tags 
	UE_DEFINE_GAMEPLAY_TAG(Input_Ability_Jump, "Input.Ability.Jump"); 
	UE_DEFINE_GAMEPLAY_TAG(Input_Ability_RollDice, "Input.Ability.RollDice");
	UE_DEFINE_GAMEPLAY_TAG(Input_Ability_Attack, "Input.Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability_Shot, "Input.Ability.Shot", "슈터 미니게임 전용 발사 어빌리티");
	
	// Character State Tags
	UE_DEFINE_GAMEPLAY_TAG(State_Character_Attacking, "State.Character.Attacking");
	UE_DEFINE_GAMEPLAY_TAG(State_Character_Death, "State.Character.Death");
	UE_DEFINE_GAMEPLAY_TAG(State_Character_Stun, "State.Character.Stun");
	
	// Event Tags 
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Health_Heal, "Event.Health.Heal", "캐릭터의 Health가 양변화 되는 경우 사용할 이벤트 태그");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_Death, "Event.Character.Death");
	UE_DEFINE_GAMEPLAY_TAG(Event_Trace_Start, "Event.Trace.Start");
	UE_DEFINE_GAMEPLAY_TAG(Event_Trace_End, "Event.Trace.End");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Shooter_Fire, "Event.Shooter.Fire", "슈터 미니게임 발사 애니메이션 몽타주에서 사용할 발사 이벤트 태그");
	
	// 아이템 관련 이벤트 태그
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Item_Activate, "Event.Item.Activate", "아이템 Ability 실행 시 전달하는 이벤트 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Item_Target_Next, "Event.Item.Target.Next", "아이템 타게팅 다음 대상 선택 입력 이벤트 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Item_Target_Previous, "Event.Item.Target.Previous", "아이템 타게팅 이전 대상 선택 입력 이벤트 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Item_Target_Confirm, "Event.Item.Target.Confirm", "아이템 타게팅 확정 입력 이벤트 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Item_Target_Cancel, "Event.Item.Target.Cancel", "아이템 타게팅 취소 입력 이벤트 태그");
	
	// Data Tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_Health_Heal, "Data.Health.Heal", "Health Attribute를 양변화 시키는 경우 사용할 태그")
	UE_DEFINE_GAMEPLAY_TAG(Data_Health_Damage, "Data.Health.Damage");
	UE_DEFINE_GAMEPLAY_TAG(Data_Point_Coin, "Data.Point.Coin");
	UE_DEFINE_GAMEPLAY_TAG(Data_Point_Trophy, "Data.Point.Trophy");

	// Ability Tile Event Tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tile_Ability_Goal, "Tile.Ability.Goal", "목표 이벤트 타일 어빌리티");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tile_Ability_Coin, "Tile.Ability.Coin", "코인 이벤트 타일 어빌리티");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tile_Ability_Item, "Tile.Ability.Item", "아이템 이벤트 타일 어빌리티");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tile_Ability_Move, "Tile.Ability.Move", "이동 이벤트 타일 어빌리티");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tile_Ability_Damage, "Tile.Ability.Damage", "데미지 이벤트 타일 어빌리티");

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

	// 미니게임 스폰 슬롯 태그
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpawnSlot_MiniGame_Slot0, "SpawnSlot.MiniGame.Slot0", "SlotIndex 0번과 대응");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpawnSlot_MiniGame_Slot1, "SpawnSlot.MiniGame.Slot1", "SlotIndex 1번과 대응");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpawnSlot_MiniGame_Slot2, "SpawnSlot.MiniGame.Slot2", "SlotIndex 2번과 대응");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpawnSlot_MiniGame_Slot3, "SpawnSlot.MiniGame.Slot3", "SlotIndex 3번과 대응");
	
	// 보드게임 로비 상태 태그 
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_LobbyWaiting, "State.BoardGame.LobbyWaiting", "로비 대기");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_LobbyStarting, "State.BoardGame.LobbyStarting", "로비 카운트다운");
    
    // 보드게임 보드판 상태 태그 
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_Init, "State.BoardGame.Init", "보드판 라운드 시작");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_PlayerTurn, "State.BoardGame.PlayerTurn", "플레이어 턴 진행");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_RoundEnd, "State.BoardGame.RoundEnd", "턴 종료 및 대기");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_HasUsedItem, "State.BoardGame.HasUsedItem", "아이템 사용한 상태");
	
    // 보드게임 종료 상태 태그
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_Ending, "State.BoardGame.Ending", "최종 승자 판정");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_End, "State.BoardGame.End", "결과창 출력");

    // 보드게임 턴 제어 상태 태그
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_TurnActive, "State.BoardGame.TurnActive", "내 턴");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BoardGame_TurnWaiting, "State.BoardGame.TurnWaiting", "남의 턴");
	
	// 보드게임 턴 내부 세부 페이즈 태그
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_TurnPhase_BeforeDice, "State.TurnPhase.BeforeDice", "주사위 굴리기 전");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_TurnPhase_Moving, "State.TurnPhase.Moving", "보드판 이동 중 (모든 조작 불가)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_TurnPhase_SelectTile, "State.TurnPhase.SelectTile", "이동 중 타일 선택 (마우스 허용)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_TurnPhase_Event, "State.TurnPhase.Event", "이동 완료 후 (상점 이용 및 트로피 획득 판정 중)");
	
	// 아이템 타입 태그
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Activate_Instant, "Item.Activate.Instant", "즉시사용 아이템");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Activate_Targeting, "Item.Activate.Targeting", "타게팅 아이템");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Activate_Range, "Item.Activate.Range", "범위 아이템");

	// 아이템 액션 어빌리티 태그
	// 아이템 어빌리티 태그
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Ability_HealingKit, "Item.Ability.HealingKit", "힐링 키트 아이템 어빌리티");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Ability_GiveBomb, "Item.Ability.GiveBomb", "폭탄 넘기기 아이템 어빌리티");
	
	// 아이템 액션 상태 태그
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_ItemAction_Targeting, "State.ItemAction.Targeting", "아이템 타겟 선택 중");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_ItemAction_Range, "State.ItemAction.Range", "아이템 범위 표시/선택 중");
	
	// UI 태그 
	UE_DEFINE_GAMEPLAY_TAG(BoardGame_UI_PlayerTurn, "BoardGame.UI.PlayerTurn"); 
	UE_DEFINE_GAMEPLAY_TAG(Lobby_UI_NicknamePopup, "Lobby.UI.NicknamePopup"); 
	UE_DEFINE_GAMEPLAY_TAG(Lobby_UI_Main, "Lobby.UI.Main"); 
	UE_DEFINE_GAMEPLAY_TAG(Lobby_UI_CountDown, "Lobby.UI.CountDown");
}

