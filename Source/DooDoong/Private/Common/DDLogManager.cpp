#include "Common/DDLogManager.h"

// 카테고리 정의
DEFINE_LOG_CATEGORY(LogJJH);
DEFINE_LOG_CATEGORY(LogLSY);
DEFINE_LOG_CATEGORY(LogKMS);
DEFINE_LOG_CATEGORY(LogKSH);
DEFINE_LOG_CATEGORY(LogPMJ);
DEFINE_LOG_CATEGORY(LogCYS);
DEFINE_LOG_CATEGORY(LogCJH);

FDDLogManager& FDDLogManager::Get()
{
	static FDDLogManager Instance;
	if (!Instance.bInitialized)
		Instance.Initialize();
	return Instance;
}

void FDDLogManager::Initialize()
{
	// 추가
	CategoryMap.Add(ELogOwner::JJH, {&LogJJH, ELogVerbosity::Log, true});
	CategoryMap.Add(ELogOwner::LSY, {&LogLSY, ELogVerbosity::Log, true});
	CategoryMap.Add(ELogOwner::KMS, {&LogKMS, ELogVerbosity::Log, true});
	CategoryMap.Add(ELogOwner::KSH, {&LogKSH, ELogVerbosity::Log, true});
	CategoryMap.Add(ELogOwner::PMJ, {&LogPMJ, ELogVerbosity::Log, true});
	CategoryMap.Add(ELogOwner::CYS, {&LogCYS, ELogVerbosity::Log, true});
	CategoryMap.Add(ELogOwner::CJH, {&LogCJH, ELogVerbosity::Log, true});

	bInitialized = true;
}

void FDDLogManager::SetEnabled(ELogOwner Owner, bool bEnable)
{
	if (FLogCategoryEntry* Entry = CategoryMap.Find(Owner))
	{
		Entry->bEnabled = bEnable;
		Entry->Category->SetVerbosity(
			bEnable ? Entry->DefaultVerbosity : ELogVerbosity::NoLogging
		);
	}
}

void FDDLogManager::ToggleAll(bool bEnable)
{
	for (auto& [Owner, Entry] : CategoryMap)
	{
		Entry.bEnabled = bEnable;
		Entry.Category->SetVerbosity(
			bEnable ? Entry.DefaultVerbosity : ELogVerbosity::NoLogging
		);
	}
}

bool FDDLogManager::IsEnabled(ELogOwner Owner) const
{
	if (const FLogCategoryEntry* Entry = CategoryMap.Find(Owner))
		return Entry->bEnabled;
	return false;
}

void FDDLogManager::SetEnabledByName(const FString& OwnerName, bool bEnable)
{
	static TMap<FString, ELogOwner> NameMap = {
		// 추가
		{TEXT("JJH"), ELogOwner::JJH},
		{TEXT("LSY"), ELogOwner::LSY},
		{TEXT("KMS"), ELogOwner::KMS},
		{TEXT("KSH"), ELogOwner::KSH},
		{TEXT("PMJ"), ELogOwner::PMJ},
		{TEXT("CYS"), ELogOwner::CYS},
		{TEXT("CJH"), ELogOwner::CJH},
	};

	if (const ELogOwner* Found = NameMap.Find(OwnerName))
		SetEnabled(*Found, bEnable);
}

void FDDLogManager::DumpStatus() const
{
	static TMap<ELogOwner, FString> Names = {
		//추가
		{ELogOwner::JJH, TEXT("JJH")},
		{ELogOwner::LSY, TEXT("LSY")},
		{ELogOwner::KMS, TEXT("KMS")},
		{ELogOwner::KSH, TEXT("KSH")},
		{ELogOwner::PMJ, TEXT("PMJ")},
		{ELogOwner::CYS, TEXT("CYS")},
		{ELogOwner::CJH, TEXT("CJH")},
	};

	for (auto& [Owner, Entry] : CategoryMap)
	{
		UE_LOG(LogTemp, Log, TEXT("[LogManager] %s: %s"),
		       *Names[Owner],
		       Entry.bEnabled ? TEXT("ON") : TEXT("OFF")
		);
	}
}
