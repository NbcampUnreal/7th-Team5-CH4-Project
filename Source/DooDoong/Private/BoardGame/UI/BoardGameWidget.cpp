#include "BoardGame/UI/BoardGameWidget.h"
#include "BoardGame/Game/DDBoardGameState.h"
#include "Components/TextBlock.h"

void UBoardGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	BindToGameState();
}

void UBoardGameWidget::NativeDestruct()
{
	if (CurrentGameState)
	{
		CurrentGameState->OnStateTimerChanged.RemoveDynamic(this, &ThisClass::UpdateTimeText);
		CurrentGameState->OnRoundChanged.RemoveDynamic(this, &ThisClass::UpdateRemainTurn);
	}
	
	Super::NativeDestruct();
}

void UBoardGameWidget::BindToGameState()
{
    CurrentGameState = GetWorld()->GetGameState<ADDBoardGameState>();

    if (CurrentGameState)
    {
        // 1. GameState를 찾음
        UE_LOG(LogTemp, Warning, TEXT("[UI] GameState 찾기 성공! 바인딩을 진행합니다."));

        CurrentGameState->OnStateTimerChanged.RemoveDynamic(this, &ThisClass::UpdateTimeText);
        CurrentGameState->OnStateTimerChanged.AddDynamic(this, &ThisClass::UpdateTimeText);
        
        CurrentGameState->OnRoundChanged.RemoveDynamic(this, &ThisClass::UpdateRemainTurn);
        CurrentGameState->OnRoundChanged.AddDynamic(this, &ThisClass::UpdateRemainTurn);

        // 2. 초기값 즉시 반영
        UpdateTimeText(CurrentGameState->StateTimer);
        UpdateRemainTurn(CurrentGameState->MaxRound - CurrentGameState->CurrentRound);
    }
    else
    {
        // 3. GameState 찾기 재시도
        UE_LOG(LogTemp, Error, TEXT("[UI] GameState가 아직 Null입니다. 0.1초 뒤 재시도합니다..."));

        FTimerHandle RetryHandle;
        GetWorld()->GetTimerManager().SetTimer(
            RetryHandle, 
            this, 
            &UBoardGameWidget::BindToGameState, 
            0.1f, 
            false
        );
    }
}

void UBoardGameWidget::UpdateTimeText(int32 InTime)
{
	UE_LOG(LogTemp, Warning, TEXT("[UI] 화면 글자 업데이트 실행! 받아온 시간: %d"), InTime);
	InTime = FMath::Max(0, InTime);
	
	if (IsValid(RemainTimeText))
	{
		FString TimeString = FString::Printf(TEXT("남은 시간 : %d"), InTime); 
		RemainTimeText->SetText(FText::FromString(TimeString));
	}
}

void UBoardGameWidget::UpdateRemainTurn(int32 InRemainTurn)
{
	InRemainTurn = FMath::Max(0, InRemainTurn);
	
	if (IsValid(RemainTurnText))
	{
		FString TurnString = FString::Printf(TEXT("남은 라운드 : %d"), InRemainTurn);
		RemainTurnText->SetText(FText::FromString(TurnString));
	}
}
