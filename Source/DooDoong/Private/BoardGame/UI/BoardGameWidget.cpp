#include "BoardGame/UI/BoardGameWidget.h"

#include "Base/Player/DDBasePlayerState.h"
#include "Blueprint/WidgetTree.h"
#include "BoardGame/Game/DDBoardGameState.h"
#include "BoardGame/UI/BoardGamePlayerInfo.h"
#include "Components/HorizontalBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

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
		CurrentGameState->OnRoundChanged.RemoveDynamic(this, &ThisClass::UpdateRemainingRound);
		CurrentGameState->OnPlayerArrayChanged.RemoveDynamic(this, &ThisClass::OnPlayerArrayChanged);
	}
	
	Super::NativeDestruct();
}

void UBoardGameWidget::BindToGameState()
{
    CurrentGameState = GetWorld()->GetGameState<ADDBoardGameState>();
	
	if (!CurrentGameState)
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
		
		return; 
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[UI] GameState 찾기 성공! 바인딩을 진행합니다."));

	// 1. 바인딩 
	CurrentGameState->OnStateTimerChanged.AddDynamic(this, &ThisClass::UpdateTimeText);
	UpdateTimeText(CurrentGameState->GetStateTimer());
	
	CurrentGameState->OnRoundChanged.AddDynamic(this, &ThisClass::UpdateRemainingRound);
	UpdateRemainingRound(CurrentGameState->MaxRound - CurrentGameState->GetCurrentRound());
	
	
	CurrentGameState->OnPlayerArrayChanged.AddDynamic(this, &ThisClass::OnPlayerArrayChanged);

	// 2. 플레이어 스테이트 목록 생성 
	if (CurrentGameState->PlayerArray.Num() > 0)
	{
		RefreshPlayerInfoWidgets();
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

void UBoardGameWidget::UpdateRemainingRound(int32 InRemainingRound)
{
	InRemainingRound = FMath::Max(0, InRemainingRound);
	
	if (IsValid(RemainingRoundText))
	{
		FString TurnString = FString::Printf(TEXT("남은 라운드 : %d"), InRemainingRound);
		RemainingRoundText->SetText(FText::FromString(TurnString));
	}
}

void UBoardGameWidget::CreatePlayerInfoWidgets(const TArray<ADDBasePlayerState*>& PlayerStates)
{
	if (!PlayerInfoContainer || !PlayerInfoWidgetClass) return;
	
	PlayerInfoContainer->ClearChildren();
	PlayerInfoWidgets.Empty();
	
	for (ADDBasePlayerState* PS : PlayerStates)
	{
		if (!PS) continue;
		
		// 1. 위젯 생성 
		UBoardGamePlayerInfo* InfoWidget =
			CreateWidget<UBoardGamePlayerInfo>(GetOwningPlayer(), PlayerInfoWidgetClass);
		if (!InfoWidget) continue;
		
		InfoWidget->InitializePlayerInfo(PS);
		
		// 2. SizeBox로 감싸기 
		USizeBox* SizeBox = WidgetTree->ConstructWidget<USizeBox>();
		SizeBox->SetWidthOverride(PlayerInfoWidth);
		SizeBox->SetHeightOverride(PlayerInfoHeight);
		SizeBox->AddChild(InfoWidget);
		
		// 3. HorizontalBox에 추가
		UVerticalBoxSlot* BoxSlot = PlayerInfoContainer->AddChildToVerticalBox(SizeBox);
		if (BoxSlot)
		{
			// 위젯 간 간격
			BoxSlot->SetPadding(FMargin(4.f, 0.f));
			BoxSlot->SetVerticalAlignment(VAlign_Center);
		}

		PlayerInfoWidgets.Add(InfoWidget);
	}
	
}

void UBoardGameWidget::OnPlayerArrayChanged()
{
	RefreshPlayerInfoWidgets();
}

void UBoardGameWidget::RefreshPlayerInfoWidgets()
{
	if (!CurrentGameState) return;

	TArray<ADDBasePlayerState*> PlayerStates;
	for (APlayerState* PS : CurrentGameState->PlayerArray)
	{
		if (ADDBasePlayerState* DDPS = Cast<ADDBasePlayerState>(PS))
			PlayerStates.Add(DDPS);
	}

	CreatePlayerInfoWidgets(PlayerStates);
}
