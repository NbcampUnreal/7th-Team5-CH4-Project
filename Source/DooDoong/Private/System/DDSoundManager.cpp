#include "System/DDSoundManager.h"

#include "Components/AudioComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "System/DDGameInstance.h"

UDDSoundManager* UDDSoundManager::Get(const UObject* WorldContext)
{
	UDDGameInstance* GameInstance = UDDGameInstance::Get(WorldContext);
	if (!GameInstance)
	{
		return nullptr;
	}

	return GameInstance->GetSubsystem<UDDSoundManager>();
}

void UDDSoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CategoryVolumes.Add(EDDSoundCategory::BGM, 1.f);
	CategoryVolumes.Add(EDDSoundCategory::SFX, 1.f);
	CategoryVolumes.Add(EDDSoundCategory::UI, 1.f);
	CategoryVolumes.Add(EDDSoundCategory::GameplayCue, 1.f);

	if (const UDDGameInstance* DDGameInstance = Cast<UDDGameInstance>(GetGameInstance()))
	{
		SoundDataTable = DDGameInstance->SoundDataTable;
	}
}

void UDDSoundManager::Deinitialize()
{
	StopBGM(0.f);
	SoundDataTable = nullptr;
	CategoryVolumes.Empty();

	Super::Deinitialize();
}

void UDDSoundManager::PlayBGM(FName SoundID, float FadeIn)
{
	const FDDSoundDataTableRow* SoundRow = FindSoundRow(SoundID);
	if (!SoundRow || !SoundRow->Sound)
	{
		return;
	}

	// 기존에 재생되던 BGM 중지
	StopBGM(SoundRow->FadeOutTime);

	CurrentBGM = UGameplayStatics::SpawnSound2D(
		GetGameInstance(),
		SoundRow->Sound,
		GetFinalVolume(*SoundRow),
		SoundRow->Pitch,
		SoundRow->StartTime,
		nullptr,
		true
	);

	if (CurrentBGM)
	{
		const float FadeInTime = ResolveFadeTime(FadeIn, SoundRow->FadeInTime);
		if (FadeInTime > 0.f)
		{
			CurrentBGM->FadeIn(FadeInTime, GetFinalVolume(*SoundRow), SoundRow->StartTime);
		}
	}
}

void UDDSoundManager::PlaySound2D(FName SoundID)
{
	const FDDSoundDataTableRow* SoundRow = FindSoundRow(SoundID);
	if (!SoundRow || !SoundRow->Sound)
	{
		return;
	}

	UGameplayStatics::PlaySound2D(
		GetGameInstance(),
		SoundRow->Sound,
		GetFinalVolume(*SoundRow),
		SoundRow->Pitch,
		SoundRow->StartTime,
		SoundRow->Concurrency
	);
}

void UDDSoundManager::PlaySoundAtLocation(FName SoundID, FVector Location)
{
	const FDDSoundDataTableRow* SoundRow = FindSoundRow(SoundID);
	if (!SoundRow || !SoundRow->Sound)
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(
		GetGameInstance(),
		SoundRow->Sound,
		Location,
		GetFinalVolume(*SoundRow),
		SoundRow->Pitch,
		SoundRow->StartTime,
		SoundRow->Attenuation,
		SoundRow->Concurrency
	);
}

void UDDSoundManager::PlaySoundAttached(FName SoundID, USceneComponent* AttachToComponent, FName SocketName)
{
	const FDDSoundDataTableRow* SoundRow = FindSoundRow(SoundID);
	if (!SoundRow || !SoundRow->Sound || !AttachToComponent)
	{
		return;
	}

	UGameplayStatics::SpawnSoundAttached(
		SoundRow->Sound,
		AttachToComponent,
		SocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true,
		GetFinalVolume(*SoundRow),
		SoundRow->Pitch,
		SoundRow->StartTime,
		SoundRow->Attenuation,
		SoundRow->Concurrency,
		true
	);
}

void UDDSoundManager::StopBGM(float FadeOut)
{
	if (!CurrentBGM)
	{
		return;
	}

	const float FadeOutTime = FMath::Max(0.f, FadeOut);
	if (FadeOutTime > 0.f)
	{
		CurrentBGM->FadeOut(FadeOutTime, 0.f);
	}
	else
	{
		CurrentBGM->Stop();
	}

	CurrentBGM = nullptr;
}

void UDDSoundManager::StopSound(FName SoundID, float FadeOut)
{
	// 1차 구현은 단발 SFX를 추적하지 않는다. BGM 정지만 지원한다.
}

void UDDSoundManager::StopCategory(EDDSoundCategory Category, float FadeOut)
{
	if (Category == EDDSoundCategory::BGM)
	{
		StopBGM(FadeOut);
	}
}

void UDDSoundManager::SetCategoryVolume(EDDSoundCategory Category, float Volume)
{
	const float ClampedVolume = FMath::Clamp(Volume, 0.f, 1.f);
	CategoryVolumes.FindOrAdd(Category) = ClampedVolume;

	if (Category == EDDSoundCategory::BGM && CurrentBGM)
	{
		CurrentBGM->SetVolumeMultiplier(ClampedVolume);
	}
}

float UDDSoundManager::GetCategoryVolume(EDDSoundCategory Category) const
{
	if (const float* Volume = CategoryVolumes.Find(Category))
	{
		return *Volume;
	}

	return 1.f;
}

const FDDSoundDataTableRow* UDDSoundManager::FindSoundRow(FName SoundID) const
{
	if (!SoundDataTable || SoundID.IsNone())
	{
		return nullptr;
	}

	return SoundDataTable->FindRow<FDDSoundDataTableRow>(SoundID, TEXT("DDSoundManager"));
}

float UDDSoundManager::GetFinalVolume(const FDDSoundDataTableRow& SoundRow) const
{
	return SoundRow.Volume * GetCategoryVolume(SoundRow.Category);
}

float UDDSoundManager::ResolveFadeTime(float OverrideFadeTime, float DefaultFadeTime) const
{
	return OverrideFadeTime >= 0.f ? OverrideFadeTime : DefaultFadeTime;
}
