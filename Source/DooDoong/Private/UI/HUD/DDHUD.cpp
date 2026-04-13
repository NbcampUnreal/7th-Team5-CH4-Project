#include "UI/HUD/DDHUD.h"
#include "UI/TestLogicManager.h"
#include "UI/Widgets/DDMiniGameReadyWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "System/MiniGame/DDMiniGameManager.h"
#include "GameFramework/GameStateBase.h"

#include "Base/Player/DDBasePlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/DDHealthSet.h"
#include "AbilitySystem/Attributes/DDPointSet.h"

#include "System/DDGameplayTags.h"





DEFINE_LOG_CATEGORY_STATIC(LogDDHUD, Log, All);

ADDHUD::ADDHUD()
{
	PrimaryActorTick.bCanEverTick = false;
}




void ADDHUD::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] BeginPlay Start"));

	UIConfig = LoadObject<UUIConfigData>(
		nullptr,
		TEXT("/Game/DooDoong/05_UI/DA_UIConfig.DA_UIConfig")
	);

	if (!UIConfig)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD] UIConfig LOAD FAILED"));
		return;
	}

	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD] No PlayerController"));
		return;
	}

	// -----------------------------
	// UI 생성
	// -----------------------------
	FString MapName = UGameplayStatics::GetCurrentLevelName(this);

	if (MapName.Contains("L_Lobby"))
	{
		ShowWidget(UIConfig->LobbyWidget);
	}
	else if (MapName.Contains("L_BaseGame"))
	{
		ShowWidget(UIConfig->BaseGameWidget);
	}
	else
	{
		ShowWidget(UIConfig->MiniGameHUDWidget);
	}

	// -----------------------------
	// GameState / GAS / PlayerState "구독만"
	// -----------------------------



	// MiniGameManager는 즉시 구독 가능
	if (UGameInstance* GI = GetGameInstance())
	{
		MiniGameManager = GI->GetSubsystem<UDDMiniGameManager>();

		if (MiniGameManager)
		{
			MiniGameManager->OnMiniGameStateChanged.AddDynamic(
				this,
				&ADDHUD::HandleMiniGameStateChanged
			);

			HandleMiniGameStateChanged(MiniGameManager->GetCurrentState());
		}
	}

}


















void ADDHUD::HandleMiniGameStateChanged(FGameplayTag NewState)
{
	UE_LOG(LogDDHUD, Warning, TEXT("===================================="));
	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] State Changed: %s"), *NewState.ToString());

	if (NewState == LastState)
	{
		UE_LOG(LogDDHUD, Warning, TEXT("[HUD] State ignored (same)"));
		return;
	}

	LastState = NewState;

	const bool bIsPreparing = (NewState == DDGameplayTags::State_MiniGame_Preparing);
	const bool bIsPlaying = (NewState == DDGameplayTags::State_MiniGame_Playing);

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] Preparing: %d"), bIsPreparing);
	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] Playing: %d"), bIsPlaying);

	// =========================
	// READY UI 처리
	// =========================

	if (bIsPreparing)
	{
		UE_LOG(LogDDHUD, Warning, TEXT("[HUD] -> SHOW READY UI"));
		ShowMiniGameReadyUI();
	}
	else if (bIsPlaying)
	{
		UE_LOG(LogDDHUD, Warning, TEXT("[HUD] -> HIDE READY UI"));
		HideMiniGameReadyUI();
	}

	UE_LOG(LogDDHUD, Warning, TEXT("===================================="));
}







void ADDHUD::ShowMiniGameReadyUI()
{
	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] ShowMiniGameReadyUI CALL"));

	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD] No PC"));
		return;
	}

	if (!UIConfig || !UIConfig->MiniGameReadyWidget)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD] UIConfig ReadyWidget NULL"));
		return;
	}

	// 이미 존재하면 재사용
	if (MiniGameReadyWidget)
	{
		if (!MiniGameReadyWidget->IsInViewport())
		{
			UE_LOG(LogDDHUD, Warning, TEXT("[HUD] Re-Add Existing Widget"));
			MiniGameReadyWidget->AddToViewport(1000);
		}
		else
		{
			UE_LOG(LogDDHUD, Warning, TEXT("[HUD] Already In Viewport"));
		}
		return;
	}

	// 위젯 생성
	UDDMiniGameReadyWidget* ReadyWidget =
		CreateWidget<UDDMiniGameReadyWidget>(
			PC,
			UIConfig->MiniGameReadyWidget
		);

	if (!ReadyWidget)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD] CreateWidget FAILED"));
		return;
	}

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] Create ReadyWidget %p"), ReadyWidget);

	// ❌ 여기 전체 삭제됨 (Definition Load + SetMiniGameData)

	// Viewport 추가 + 저장
	MiniGameReadyWidget = ReadyWidget;
	MiniGameReadyWidget->AddToViewport(1000);

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] Ready UI SHOWN"));
}
























void ADDHUD::HideMainWidget()
{
	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] HideMainWidget CALL"));

	if (MainWidgetInstance)
	{
		MainWidgetInstance->RemoveFromParent();
		MainWidgetInstance = nullptr;

		UE_LOG(LogDDHUD, Warning, TEXT("[HUD] MainWidget Removed"));
	}
}





void ADDHUD::ShowWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] ShowWidget ENTER"));

	if (!WidgetClass)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD] WidgetClass NULL"));
		return;
	}

	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD] PC NULL"));
		return;
	}

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] Creating Widget: %s"),
		*WidgetClass->GetName());

	if (MainWidgetInstance)
	{
		UE_LOG(LogDDHUD, Warning, TEXT("[HUD] Removing Old Widget"));
		MainWidgetInstance->RemoveFromParent();
		MainWidgetInstance = nullptr;
	}

	MainWidgetInstance = CreateWidget<UUserWidget>(PC, WidgetClass);

	if (!MainWidgetInstance)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD] CreateWidget FAILED"));
		return;
	}

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] Widget CREATED OK"));

	MainWidgetInstance->AddToViewport();

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] Widget ADDED TO VIEWPORT"));



	// =========================
// 🔥 여기부터 추가
// =========================


	UBaseGameWidget* GameWidget = Cast<UBaseGameWidget>(MainWidgetInstance);

	if (GameWidget)
	{
		UE_LOG(LogDDHUD, Warning, TEXT("🔥 INSIDE IF BLOCK"));

		UE_LOG(LogDDHUD, Warning, TEXT("[HUD] BaseGameWidget Detected"));

		AGameStateBase* GS = GetWorld()->GetGameState();

		if (!GS)
		{
			UE_LOG(LogDDHUD, Error, TEXT("[HUD] GameState NULL"));
			return;
		}

		UE_LOG(LogDDHUD, Warning, TEXT("[HUD] Calling InitHUD, PlayerNum: %d"), GS->PlayerArray.Num());

		//GameWidget->InitHUD(GS->PlayerArray);
	}
}














void ADDHUD::HideMiniGameReadyUI()
{
	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] HideMiniGameReadyUI CALL"));

	if (!UIConfig || !UIConfig->MiniGameReadyWidget)
		return;

	//  현재 월드의 모든 위젯 찾기
	TArray<UUserWidget*> FoundWidgets;

	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
		GetWorld(),
		FoundWidgets,
		UIConfig->MiniGameReadyWidget,
		false
	);

	// 👉 전부 제거
	for (UUserWidget* Widget : FoundWidgets)
	{
		if (Widget)
		{
			UE_LOG(LogDDHUD, Warning, TEXT("[HUD] Removing Widget %p"), Widget);
			Widget->RemoveFromParent();
		}
	}

	MiniGameReadyWidget = nullptr;
}
































