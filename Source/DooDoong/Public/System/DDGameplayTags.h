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
}