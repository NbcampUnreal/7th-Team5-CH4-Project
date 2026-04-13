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

	BindToPlayerState();

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
	BindToGameState();

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

	// =========================
	// 1. PlayerController 체크
	// =========================
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD] No PC"));
		return;
	}

	// =========================
	// 2. UIConfig 체크
	// =========================
	if (!UIConfig || !UIConfig->MiniGameReadyWidget)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD] UIConfig ReadyWidget NULL"));
		return;
	}

	// =========================
	// 3. 이미 존재하면 재사용 (중요)
	// =========================
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

	// =========================
	// 4. 위젯 생성
	// =========================
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

	// =========================
	// 5. (디버그용) DataAsset 하드코딩 로드
	// =========================
	UDDMiniGameDefinition* Definition =
		LoadObject<UDDMiniGameDefinition>(
			nullptr,
			TEXT("/Game/DooDoong/04_MiniGames/Platformer/Data/DA_Platformer.DA_Platformer")
		);

	if (!Definition)
	{
		UE_LOG(LogTemp, Error, TEXT("[HUD] Definition LOAD FAIL"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[HUD] Definition LOAD SUCCESS"));
		UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *Definition->DisplayName.ToString());

		ReadyWidget->SetMiniGameData(Definition);
	}

	// =========================
	// 6. Viewport 추가 + 저장
	// =========================
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

		GameWidget->InitHUD(GS->PlayerArray);
	}
}






void ADDHUD::HandleReadyStateChanged(int32 ReadyCount, int32 TotalCount)
{
	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] Ready: %d / %d"), ReadyCount, TotalCount);

	if (MiniGameReadyWidget)
	{
		MiniGameReadyWidget->SetReadyCount(ReadyCount, TotalCount);
	}

	if (TotalCount > 0 && ReadyCount >= TotalCount)
	{
		UE_LOG(LogDDHUD, Warning, TEXT("[HUD] All Ready → Hide Ready UI"));

		HideMiniGameReadyUI();
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










void ADDHUD::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	//UpdateHealth(Data.NewValue);
}

void ADDHUD::OnCoinChanged(const FOnAttributeChangeData& Data)
{
	//UpdateGold((int32)Data.NewValue);
}




void ADDHUD::TryBindGAS()
{
	UE_LOG(LogDDHUD, Warning, TEXT("[GAS] TryBindGAS ENTER"));

	APlayerController* PC = GetOwningPlayerController();

	if (!PC)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[GAS] PC NULL"));
		return;
	}
	UE_LOG(LogDDHUD, Warning, TEXT("[GAS] PC OK"));



	ADDBasePlayerState* PS = PC->GetPlayerState<ADDBasePlayerState>();
	if (!PS)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[GAS] PlayerState NOT READY -> retry next tick"));
		return;
	}
	UE_LOG(LogDDHUD, Warning, TEXT("[GAS] PlayerState OK"));



	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[GAS] ASC NULL"));
		return;
	}
	UE_LOG(LogDDHUD, Warning, TEXT("[GAS] ASC OK"));



	const UDDHealthSet* HealthSet = PS->GetHealthSet();
	if (!HealthSet)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[GAS] HealthSet NULL"));
		return;
	}
	UE_LOG(LogDDHUD, Warning, TEXT("[GAS] HealthSet OK"));



	UE_LOG(LogDDHUD, Warning, TEXT("[GAS] READY TO BIND Delegate"));

	ASC->GetGameplayAttributeValueChangeDelegate(
		HealthSet->GetHealthAttribute()
	).AddUObject(this, &ADDHUD::OnHealthChanged);

	UE_LOG(LogDDHUD, Warning, TEXT("[GAS] BIND COMPLETE"));



	// =========================
	// INITIAL SYNC (핵심)
	// =========================
	float CurrentHealth = ASC->GetNumericAttribute(
		HealthSet->GetHealthAttribute()
	);

	UE_LOG(LogDDHUD, Warning, TEXT("[GAS] INIT HEALTH = %f"), CurrentHealth);

	ApplyInitialHealth(CurrentHealth);
}


void ADDHUD::ApplyInitialHealth(float Value)
{
	int32 InitHealth = FMath::RoundToInt(Value);

	UE_LOG(LogDDHUD, Warning, TEXT("[GAS] INIT SYNC Health = %d"), InitHealth);

	if (TestLogicManager)
	{
		TestLogicManager->SetHealth(0, InitHealth);
	}
}







void ADDHUD::BindToGameState()
{
	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] BindToGameState ENTER"));

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ADDMiniGameStateBase* GS = World->GetGameState<ADDMiniGameStateBase>();
	if (!GS)
	{
		UE_LOG(LogDDHUD, Warning, TEXT("[HUD] GameState NOT READY -> retry"));

		World->GetTimerManager().SetTimerForNextTick(
			this,
			&ADDHUD::BindToGameState
		);
		return;
	}

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] GameState READY"));

	// 중복 바인딩 방지
	GS->OnMiniGameReadyStateChanged.RemoveAll(this);

	GS->OnMiniGameReadyStateChanged.AddDynamic(
		this,
		&ADDHUD::HandleReadyStateChanged
	);

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] ReadyState Delegate BOUND"));

	// ✔ 초기 sync (Getter 사용)
	HandleReadyStateChanged(
		GS->GetReadyPlayerCount(),
		GS->GetTotalParticipantCount()
	);
}



void ADDHUD::UpdateGold(int32 Value)
{

}

void ADDHUD::UpdateHealth(float Value)
{

}













void ADDHUD::BindToPlayerState()
{
	
	APlayerController* PC = GetOwningPlayerController();

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD][PS] BindToPlayerState ENTER"));

	if (!PC)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD][PS] PC NULL"));
		return;
	}

	ADDBasePlayerState* PS = PC->GetPlayerState<ADDBasePlayerState>();

	if (!PS)
	{
		UE_LOG(LogDDHUD, Warning, TEXT("[HUD][PS] PlayerState NOT READY -> retry next tick"));

		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this,
			&ADDHUD::BindToPlayerState
		);
		return;
	}

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD][PS] PlayerState READY: %s"), *PS->GetPlayerName());

	PS->OnGASReady.AddUObject(this, &ADDHUD::OnGASReady);

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD][PS] GAS Delegate BOUND"));



}














void ADDHUD::OnGASReady()
{
	UE_LOG(LogDDHUD, Warning, TEXT("================================================"));
	UE_LOG(LogDDHUD, Warning, TEXT("[HUD][GAS] OnGASReady TRIGGERED"));

	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD][GAS] PC NULL"));
		return;
	}

	ADDBasePlayerState* PS = PC->GetPlayerState<ADDBasePlayerState>();
	if (!PS)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD][GAS] PS NULL"));
		return;
	}

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD][GAS] PS OK"));

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD][GAS] ASC NULL"));
		return;
	}

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD][GAS] ASC OK"));

	const UDDHealthSet* HealthSet = PS->GetHealthSet();
	const UDDPointSet* PointSet = PS->GetPointSet();

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD][GAS] HealthSet=%s PointSet=%s"),
		HealthSet ? TEXT("OK") : TEXT("NULL"),
		PointSet ? TEXT("OK") : TEXT("NULL"));

	if (!HealthSet || !PointSet)
		return;

	// 초기값
	float Health = ASC->GetNumericAttribute(HealthSet->GetHealthAttribute());
	float Coin = ASC->GetNumericAttribute(PointSet->GetCoinAttribute());

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD][GAS] INIT Health=%f Coin=%f"), Health, Coin);

	UpdateHealth(Health);
	UpdateGold((int32)Coin);

	// Delegate
	ASC->GetGameplayAttributeValueChangeDelegate(
		HealthSet->GetHealthAttribute()
	).AddUObject(this, &ADDHUD::OnHealthChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		PointSet->GetCoinAttribute()
	).AddUObject(this, &ADDHUD::OnCoinChanged);

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD][GAS] Delegate BIND COMPLETE"));
	UE_LOG(LogDDHUD, Warning, TEXT("================================================"));
}




void ADDHUD::InitHUDDelayed()
{
	if (!MainWidgetInstance) return;

	UBaseGameWidget* GameWidget = Cast<UBaseGameWidget>(MainWidgetInstance);
	if (!GameWidget) return;

	AGameStateBase* GS = GetWorld()->GetGameState();
	if (!GS)
	{
		UE_LOG(LogDDHUD, Error, TEXT("[HUD] GameState NULL"));
		return;
	}

	UE_LOG(LogDDHUD, Warning, TEXT("[HUD] Calling InitHUD, PlayerNum: %d"), GS->PlayerArray.Num());

	GameWidget->InitHUD(GS->PlayerArray);
}