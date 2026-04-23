#include "System/DDSoundManager.h"

#include "Components/AudioComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/ConfigCacheIni.h"
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

	if (const UDDGameInstance* DDGameInstance = Cast<UDDGameInstance>(GetGameInstance()))
	{
		SoundDataTable = DDGameInstance->SoundDataTable;
	}

	LoadAudioSettings();
}

void UDDSoundManager::Deinitialize()
{
	StopBGM(0.f);
	SoundDataTable = nullptr;

	Super::Deinitialize();
}

void UDDSoundManager::PlayBGM(FName SoundID, float FadeIn)
{
	if (CurrentBGM && CurrentBGMID == SoundID)
	{
		return;
	}
	
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
		CurrentBGMID = SoundID;
		CurrentBGM->OnAudioFinished.AddUniqueDynamic(this, &UDDSoundManager::HandleBGMFinished);
		
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
		CurrentBGMID = NAME_None;
		return;
	}

	const float FadeOutTime = FMath::Max(0.f, FadeOut);
	CurrentBGM->OnAudioFinished.RemoveAll(this);
	if (FadeOutTime > 0.f)
	{
		CurrentBGM->FadeOut(FadeOutTime, 0.f);
	}
	else
	{
		CurrentBGM->Stop();
	}

	CurrentBGM = nullptr;
	CurrentBGMID = NAME_None;
}

void UDDSoundManager::StopSound(FName SoundID, float FadeOut)
{
	// 1차 구현은 단발 SFX를 추적하지 않는다. BGM 정지만 지원한다.
}

void UDDSoundManager::HandleBGMFinished()
{
	const FName FinishedBGMID = CurrentBGMID;
	CurrentBGM = nullptr;
	CurrentBGMID = NAME_None;

	const FDDSoundDataTableRow* SoundRow = FindSoundRow(FinishedBGMID);
	if (SoundRow && SoundRow->bLoop)
	{
		PlayBGM(FinishedBGMID, 0.f);
	}
}

void UDDSoundManager::StopCategory(EDDSoundCategory Category, float FadeOut)
{
	if (Category == EDDSoundCategory::BGM)
	{
		StopBGM(FadeOut);
	}
}

void UDDSoundManager::SetMasterVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.f, 1.f);

	if (CurrentBGM && !CurrentBGMID.IsNone())
	{
		if (const FDDSoundDataTableRow* SoundRow = FindSoundRow(CurrentBGMID))
		{
			CurrentBGM->SetVolumeMultiplier(GetFinalVolume(*SoundRow));
		}
	}
}

void UDDSoundManager::SetCategoryVolume(EDDSoundCategory Category, float Volume)
{
	const float ClampedVolume = FMath::Clamp(Volume, 0.f, 1.f);
	switch (Category)
	{
	case EDDSoundCategory::BGM:
		BGMVolume = ClampedVolume;
		break;
	case EDDSoundCategory::SFX:
		SFXVolume = ClampedVolume;
		break;
	case EDDSoundCategory::UI:
		InterfaceVolume = ClampedVolume;
		break;
	default:
		break;
	}

	if (Category == EDDSoundCategory::BGM && CurrentBGM)
	{
		if (const FDDSoundDataTableRow* SoundRow = FindSoundRow(CurrentBGMID))
		{
			CurrentBGM->SetVolumeMultiplier(GetFinalVolume(*SoundRow));
		}
	}
}

float UDDSoundManager::GetCategoryVolume(EDDSoundCategory Category) const
{
	switch (Category)
	{
	case EDDSoundCategory::BGM:
		return BGMVolume;
	case EDDSoundCategory::SFX:
		return SFXVolume;
	case EDDSoundCategory::UI:
		return InterfaceVolume;
	default:
		return 1.f;
	}
}

void UDDSoundManager::SaveAudioSettings()
{
	if (GConfig)
	{
		const FString Section = TEXT("/Script/DOODOONG.AudioSettings");
		GConfig->SetFloat(*Section, TEXT("MasterVolume"), MasterVolume, GGameUserSettingsIni);
		GConfig->SetFloat(*Section, TEXT("BGMVolume"), BGMVolume, GGameUserSettingsIni);
		GConfig->SetFloat(*Section, TEXT("SFXVolume"), SFXVolume, GGameUserSettingsIni);
		GConfig->SetFloat(*Section, TEXT("InterfaceVolume"), InterfaceVolume, GGameUserSettingsIni);
		GConfig->Flush(false, GGameUserSettingsIni);
	}
}

void UDDSoundManager::LoadAudioSettings()
{
	if (!GConfig)
	{
		return;
	}

	const FString Section = TEXT("/Script/DOODOONG.AudioSettings");
	float SavedVolume = 1.f;

	if (GConfig->GetFloat(*Section, TEXT("MasterVolume"), SavedVolume, GGameUserSettingsIni))
	{
		SetMasterVolume(SavedVolume);
	}
	if (GConfig->GetFloat(*Section, TEXT("BGMVolume"), SavedVolume, GGameUserSettingsIni))
	{
		SetCategoryVolume(EDDSoundCategory::BGM, SavedVolume);
	}
	if (GConfig->GetFloat(*Section, TEXT("SFXVolume"), SavedVolume, GGameUserSettingsIni))
	{
		SetCategoryVolume(EDDSoundCategory::SFX, SavedVolume);
	}
	if (GConfig->GetFloat(*Section, TEXT("InterfaceVolume"), SavedVolume, GGameUserSettingsIni))
	{
		SetCategoryVolume(EDDSoundCategory::UI, SavedVolume);
	}
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
	return SoundRow.Volume * MasterVolume * GetCategoryVolume(SoundRow.Category);
}

float UDDSoundManager::ResolveFadeTime(float OverrideFadeTime, float DefaultFadeTime) const
{
	return OverrideFadeTime >= 0.f ? OverrideFadeTime : DefaultFadeTime;
}
