#include "System/DDGameInstance.h"

#include "Common/DDLogManager.h"
#include "Data/DDUIConfig.h"
#include "Abilities/GameplayAbility.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemNames.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

void UDDGameInstance::Init()
{
	Super::Init();
	
	FDDLogManager::Get().ToggleAll(false);

	int32 ValidCount = 0;
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : PreloadedAbilityClasses)
	{
		if (*AbilityClass) { ++ValidCount; }
	}

	if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			OnSessionInviteAcceptedHandle = SessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(
				FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &ThisClass::OnSessionInviteAccepted));
		}
	}

	TryCreateDedicatedServerSession();
}

void UDDGameInstance::Shutdown()
{
	if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			if (OnCreateSessionCompleteHandle.IsValid())
			{
				SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);
				OnCreateSessionCompleteHandle.Reset();
			}

			if (OnSessionInviteAcceptedHandle.IsValid())
			{
				SessionInterface->ClearOnSessionUserInviteAcceptedDelegate_Handle(OnSessionInviteAcceptedHandle);
				OnSessionInviteAcceptedHandle.Reset();
			}
		}
	}

	Super::Shutdown();
}

UDDGameInstance* UDDGameInstance::Get(const UObject* WorldContext)
{
	return Cast<UDDGameInstance>(UGameplayStatics::GetGameInstance(WorldContext));
}

UDDUIConfig* UDDGameInstance::GetOrLoadUIConfig(const FSoftObjectPath& UIConfigPath)
{
	if (!UIConfigPath.IsValid())
	{
		LOG_JJH(Warning, TEXT("[GameInstance] UIConfig 경로가 존재함"));
		return nullptr;
	}

	const FString PathString = UIConfigPath.ToString();
	if (TObjectPtr<UDDUIConfig>* CachedConfig = CachedUIConfigs.Find(PathString))
	{
		if (IsValid(CachedConfig->Get()))
		{
			return CachedConfig->Get();
		}
	}

	UDDUIConfig* LoadedConfig = Cast<UDDUIConfig>(UIConfigPath.TryLoad());
	if (!LoadedConfig)
	{
		LOG_JJH(Warning, TEXT("[GameInstance] UI Config 로드 실패. Path=%s"), *PathString);
		return nullptr;
	}

	CachedUIConfigs.Add(PathString, LoadedConfig);
	LOG_JJH(Warning, TEXT("[GameInstance] UI Config 캐싱. Path=%s Config=%s"),
		*PathString,
		*GetNameSafe(LoadedConfig));
	return LoadedConfig;
}

void UDDGameInstance::ConnectToDedicatedServer(APlayerController* PlayerController) const
{
	if (!IsValid(PlayerController))
	{
		return;
	}

	PlayerController->ClientTravel(GetDedicatedServerTravelURL(), TRAVEL_Absolute);
}

bool UDDGameInstance::ShowSteamInviteOverlay(APlayerController* PlayerController) const
{
	if (!IsValid(PlayerController))
	{
		return false;
	}

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (OnlineSubsystem == nullptr)
	{
		OnlineSubsystem = IOnlineSubsystem::Get();
	}

	if (OnlineSubsystem == nullptr)
	{
		return false;
	}

	const IOnlineExternalUIPtr ExternalUI = OnlineSubsystem->GetExternalUIInterface();
	if (!ExternalUI.IsValid())
	{
		return false;
	}

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	const int32 ControllerId = LocalPlayer != nullptr ? LocalPlayer->GetControllerId() : 0;
	return ExternalUI->ShowInviteUI(ControllerId, NAME_GameSession);
}

FString UDDGameInstance::GetDedicatedServerTravelURL() const
{
	if (DedicatedServerPort > 0)
	{
		return FString::Printf(TEXT("%s:%d"), *DedicatedServerAddress, DedicatedServerPort);
	}

	return DedicatedServerAddress;
}

void UDDGameInstance::SetDedicatedServerEndpoint(const FString& InAddress, int32 InPort)
{
	DedicatedServerAddress = InAddress.TrimStartAndEnd();
	DedicatedServerPort = FMath::Max(0, InPort);
}

void UDDGameInstance::TryCreateDedicatedServerSession()
{
	if (!IsRunningDedicatedServer())
	{
		return;
	}

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (OnlineSubsystem == nullptr)
	{
		OnlineSubsystem = IOnlineSubsystem::Get();
	}

	if (OnlineSubsystem == nullptr)
	{
		return;
	}

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		return;
	}

	if (SessionInterface->GetNamedSession(NAME_GameSession) != nullptr)
	{
		return;
	}

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsDedicated = true;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.NumPublicConnections = DedicatedServerMaxPlayers;
	SessionSettings.NumPrivateConnections = 0;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowInvites = true;
	SessionSettings.bUsesPresence = false;
	SessionSettings.bAllowJoinViaPresence = false;
	SessionSettings.bUseLobbiesIfAvailable = false;
	SessionSettings.Set(SETTING_MAPNAME, FString(TEXT("L_Lobby")), EOnlineDataAdvertisementType::ViaOnlineService);

	OnCreateSessionCompleteHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateDedicatedServerSessionComplete));

	if (!SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);
		OnCreateSessionCompleteHandle.Reset();
	}
}

void UDDGameInstance::OnCreateDedicatedServerSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			if (OnCreateSessionCompleteHandle.IsValid())
			{
				SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);
				OnCreateSessionCompleteHandle.Reset();
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Dedicated server Steam session '%s' create result: %s"),
		*SessionName.ToString(),
		bWasSuccessful ? TEXT("Success") : TEXT("Failed"));
}

void UDDGameInstance::OnSessionInviteAccepted(bool bWasSuccessful, int32 ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult)
{
	if (!bWasSuccessful)
	{
		return;
	}

	if (APlayerController* PlayerController = GetFirstLocalPlayerController())
	{
		ConnectToDedicatedServer(PlayerController);
	}
}
