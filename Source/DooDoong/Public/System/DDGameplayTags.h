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
}

namespace DDMiniGameplayTags
{
	// 미니게임 상태 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Idle);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Preparing);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Traveling);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Playing);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Finishing);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Returning);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Completed);

	// 미니게임 장르 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Genre_None);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Genre_Competition);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Genre_Survival);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Genre_Race);

	// 미니게임 종료 조건 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(FinishReason_TimeOver);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(FinishReason_Completed);
}