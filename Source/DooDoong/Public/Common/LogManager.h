#pragma once
#include "CoreMinimal.h"
#include "Logging/LogVerbosity.h"

// =============================
// Log Categories
// =============================
DECLARE_LOG_CATEGORY_EXTERN(LogJJH, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogLSY, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogKMS, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogKSH, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogPMJ, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCYS, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCJH, Log, All);

// =============================
// Log Manager
// =============================
enum class ELogOwner : uint8
{
    JJH, 
    LSY, 
    KMS, 
    KSH, 
    PMJ, 
    CYS, 
    CJH,
    COUNT
};

struct FLogCategoryEntry
{
    FLogCategoryBase* Category;
    ELogVerbosity::Type DefaultVerbosity;
    bool bEnabled;
};

class DOODOONG_API FLogManager
{
public:
    static FLogManager& Get();

    void Initialize();

    void SetEnabled(ELogOwner Owner, bool bEnable);
    void ToggleAll(bool bEnable);
    bool IsEnabled(ELogOwner Owner) const;

    // 편의용: 이름으로 토글
    void SetEnabledByName(const FString& OwnerName, bool bEnable);

    // 현재 상태 출력 (디버깅용)
    void DumpStatus() const;

private:
    FLogManager() = default;

    TMap<ELogOwner, FLogCategoryEntry> CategoryMap;
    bool bInitialized = false;
};

// =============================
// 매크로 래퍼
// =============================
#define LOG_JJH(Verbosity, Format, ...) \
if (FLogManager::Get().IsEnabled(ELogOwner::JJH)) \
UE_LOG(LogJJH, Verbosity, Format, ##__VA_ARGS__)

#define LOG_LSY(Verbosity, Format, ...) \
if (FLogManager::Get().IsEnabled(ELogOwner::LSY)) \
UE_LOG(LogLSY, Verbosity, Format, ##__VA_ARGS__)

#define LOG_KMS(Verbosity, Format, ...) \
if (FLogManager::Get().IsEnabled(ELogOwner::KMS)) \
UE_LOG(LogKMS, Verbosity, Format, ##__VA_ARGS__)

#define LOG_KSH(Verbosity, Format, ...) \
if (FLogManager::Get().IsEnabled(ELogOwner::KSH)) \
UE_LOG(LogKSH, Verbosity, Format, ##__VA_ARGS__)

#define LOG_PMJ(Verbosity, Format, ...) \
if (FLogManager::Get().IsEnabled(ELogOwner::PMJ)) \
UE_LOG(LogPMJ, Verbosity, Format, ##__VA_ARGS__)

#define LOG_CYS(Verbosity, Format, ...) \
if (FLogManager::Get().IsEnabled(ELogOwner::CYS)) \
UE_LOG(LogCYS, Verbosity, Format, ##__VA_ARGS__)

#define LOG_CJH(Verbosity, Format, ...) \
if (FLogManager::Get().IsEnabled(ELogOwner::CJH)) \
UE_LOG(LogCJH, Verbosity, Format, ##__VA_ARGS__)