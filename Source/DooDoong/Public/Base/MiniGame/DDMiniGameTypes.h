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
	int32 PlayerId = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Id = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName DisplayName = NAME_None;

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
	int32 PlayerId = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	FName DisplayName = NAME_None;

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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BGM;
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

USTRUCT(BlueprintType)
struct DOODOONG_API FMiniGameReturnTileInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	int32 PlayerId = INDEX_NONE;
	
	UPROPERTY(BlueprintReadOnly)
	FName TileRowName = NAME_None;
};

USTRUCT(BlueprintType)
struct FMiniGameReadyEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="MiniGame|Ready")
	int32 PlayerId = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category="MiniGame|Ready")
	FName DisplayName = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category="MiniGame|Ready")
	bool bReady = false;
};
