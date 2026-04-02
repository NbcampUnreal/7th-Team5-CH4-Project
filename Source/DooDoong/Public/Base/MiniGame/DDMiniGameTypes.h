#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "System/DDGameplayTags.h"
#include "DDMiniGameTypes.generated.h"

USTRUCT(BlueprintType)
struct DOODOONG_API FMiniGameParticipantInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Id = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bConnected = true;
};

USTRUCT(BlueprintType)
struct DOODOONG_API FMiniGameScoreEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Score = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Rank = 0;
};

USTRUCT(BlueprintType)
struct DOODOONG_API FMiniGameSetup
{
	GENERATED_BODY()
	
	FMiniGameSetup()
	: Genre(DDGameplayTags::Genre_MiniGame_None)
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MiniGameID = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="MiniGame.Genre"))
	FGameplayTag Genre;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeLimitSeconds = 60.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer MiniGameTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> MiniGameMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ReturnContextId = NAME_None;
};

USTRUCT(BlueprintType)
struct DOODOONG_API FMiniGameResult
{
	GENERATED_BODY()

	FMiniGameResult()
		: FinishReason(DDGameplayTags::FinishReason_MiniGame_TimeOver)
	{
	}

	UPROPERTY(BlueprintReadOnly)
	FName MiniGameId = NAME_None;

	UPROPERTY(BlueprintReadOnly, meta=(Categories="MiniGame.FinishReason"))
	FGameplayTag FinishReason;

	UPROPERTY(BlueprintReadOnly)
	TArray<FMiniGameScoreEntry> ScoreBoard;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<APlayerState>> Ranking;
};