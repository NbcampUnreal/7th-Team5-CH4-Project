#include "System/DDGameInstance.h"

#include "Common/DDLogManager.h"
#include "Data/DDUIConfig.h"
#include "Abilities/GameplayAbility.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UDDGameInstance::Init()
{
	Super::Init();
	
	FDDLogManager::Get().ToggleAll(false);

	int32 ValidCount = 0;
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : PreloadedAbilityClasses)
	{
		if (*AbilityClass) { ++ValidCount; }
	}

}

void UDDGameInstance::Shutdown()
{
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
