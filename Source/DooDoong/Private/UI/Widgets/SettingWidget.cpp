#include "UI/Widgets/SettingWidget.h"

#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameUserSettings.h"
#include "System/DDGameplayTags.h"
#include "System/DDSoundManager.h"
#include "System/DDUIManagerSubsystem.h"

void USettingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->OnValueChanged.AddDynamic(this, &USettingWidget::OnMasterVolumeChanged);
	}
	if (BGMVolumeSlider)
	{
		BGMVolumeSlider->OnValueChanged.AddDynamic(this, &USettingWidget::OnBGMVolumeChanged);
	}
	if (SFXVolumeSlider)
	{
		SFXVolumeSlider->OnValueChanged.AddDynamic(this, &USettingWidget::OnSFXVolumeChanged);
	}
	
	if (ApplyButton)
	{
		ApplyButton->OnClicked.AddDynamic(this, &USettingWidget::OnApplyClicked);
	}
	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &USettingWidget::OnBackClicked);
	}
	
	InitResolutionOption();
	InitWindowModeOption();
	LoadCurrentSettings();
}

void USettingWidget::OnMasterVolumeChanged(float Value)
{
	UpdateVolumeText(MasterVolumeText, Value);
	if (UDDSoundManager* SoundManager = UDDSoundManager::Get(this))
	{
		SoundManager->SetMasterVolume(Value);
	}
}

void USettingWidget::OnBGMVolumeChanged(float Value)
{
	UpdateVolumeText(BGMVolumeText, Value);
	if (UDDSoundManager* SoundMgr = UDDSoundManager::Get(this))
	{
		SoundMgr->SetCategoryVolume(EDDSoundCategory::BGM, Value);
	}
}

void USettingWidget::OnSFXVolumeChanged(float Value)
{
	UpdateVolumeText(SFXVolumeText, Value);
	if (UDDSoundManager* SoundMgr = UDDSoundManager::Get(this))
	{
		SoundMgr->SetCategoryVolume(EDDSoundCategory::SFX, Value);
	}
}

void USettingWidget::OnInterfaceVolumeChanged(float Value)
{
	UpdateVolumeText(InterfaceVolumeText, Value);
	if (UDDSoundManager* SoundMgr = UDDSoundManager::Get(this))
	{
		SoundMgr->SetCategoryVolume(EDDSoundCategory::UI, Value);
	}
}

void USettingWidget::OnApplyClicked()
{
	if (UDDSoundManager* SoundManager = UDDSoundManager::Get(this))
	{
		SoundManager->SaveAudioSettings();
		SoundManager->PlaySound2D("UI_Click");
	}
	
	UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings();
	if (GameSettings)
	{
		if (ResolutionComboBox)
		{
			const int32 SelectedIndex = ResolutionComboBox->GetSelectedIndex();
			if (Resolutions.IsValidIndex(SelectedIndex))
			{
				GameSettings->SetScreenResolution(FIntPoint(Resolutions[SelectedIndex].Width, Resolutions[SelectedIndex].Height));
			}
		}
		
		if (WindowModeComboBox)
		{
			const int32 ModeIndex = WindowModeComboBox->GetSelectedIndex();
			switch (ModeIndex)
			{
			case 0:
				GameSettings->SetFullscreenMode(EWindowMode::Fullscreen);
				break;
			case 1:
				GameSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
				break;
			case 2:
				GameSettings->SetFullscreenMode(EWindowMode::Windowed);
				break;
			default:
				break;
			}
		}
		
		GameSettings->ApplySettings(false);
		GameSettings->SaveSettings();
	}
}

void USettingWidget::OnBackClicked()
{
	if (UDDSoundManager* SoundManager = UDDSoundManager::Get(this))
	{
		SoundManager->PlaySound2D("UI_Click");
	}
	
	if (UDDUIManagerSubsystem* UIManager = GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->GetSubsystem<UDDUIManagerSubsystem>())
	{
		UIManager->HidePopup(DDGameplayTags::Settings_UI);
	}
}

void USettingWidget::UpdateVolumeText(UTextBlock* Text, float Value)
{
	if (Text)
	{
		Text->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Value * 100.f))));
	}
}

void USettingWidget::InitResolutionOption()
{
	Resolutions.Empty();
	Resolutions.Add({1280, 720});
	Resolutions.Add({1920, 1080});
	Resolutions.Add({2560, 1440});
	Resolutions.Add({3840, 2160});

	if (ResolutionComboBox)
	{
		ResolutionComboBox->ClearOptions();
		for (const auto& Res : Resolutions)
		{
			ResolutionComboBox->AddOption(Res.ToString());
		}
	}
}

void USettingWidget::InitWindowModeOption()
{
	if (WindowModeComboBox)
	{
		WindowModeComboBox->ClearOptions();
		WindowModeComboBox->AddOption(TEXT("Fullscreen"));
		WindowModeComboBox->AddOption(TEXT("Windowed Fullscreen"));
		WindowModeComboBox->AddOption(TEXT("Windowed"));
	}
}

void USettingWidget::LoadCurrentSettings()
{
	UDDSoundManager* SoundManager = UDDSoundManager::Get(this);
	if (SoundManager)
	{
		if (MasterVolumeSlider)
		{
			MasterVolumeSlider->SetValue(SoundManager->GetMasterVolume());
		}
		if (BGMVolumeSlider)
		{
			BGMVolumeSlider->SetValue(SoundManager->GetCategoryVolume(EDDSoundCategory::BGM));
		}
		if (SFXVolumeSlider)
		{
			SFXVolumeSlider->SetValue(SoundManager->GetCategoryVolume(EDDSoundCategory::SFX));
		}
		if (InterfaceVolumeSlider)
		{
			InterfaceVolumeSlider->SetValue(SoundManager->GetCategoryVolume(EDDSoundCategory::UI));
		}

		UpdateVolumeText(MasterVolumeText, SoundManager->GetMasterVolume());
		UpdateVolumeText(BGMVolumeText, SoundManager->GetCategoryVolume(EDDSoundCategory::BGM));
		UpdateVolumeText(SFXVolumeText, SoundManager->GetCategoryVolume(EDDSoundCategory::SFX));
		UpdateVolumeText(InterfaceVolumeText, SoundManager->GetCategoryVolume(EDDSoundCategory::UI));
	}
	
	UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings();
	if (GameSettings)
	{
		const FIntPoint CurrentRes = GameSettings->GetScreenResolution();

		if (ResolutionComboBox)
		{
			for (int32 i = 0; i < Resolutions.Num(); i++)
			{
				if (Resolutions[i].Width == CurrentRes.X && Resolutions[i].Height == CurrentRes.Y)
				{
					ResolutionComboBox->SetSelectedIndex(i);
					break;
				}
			}
		}

		if (WindowModeComboBox)
		{
			const EWindowMode::Type CurrentMode = GameSettings->GetFullscreenMode();
			switch (CurrentMode)
			{
			case EWindowMode::Fullscreen:
				WindowModeComboBox->SetSelectedIndex(0);
				break;
			case EWindowMode::WindowedFullscreen:
				WindowModeComboBox->SetSelectedIndex(1);
				break;
			case EWindowMode::Windowed:
				WindowModeComboBox->SetSelectedIndex(2);
				break;
			default:
				break;
			}
		}
	}
}
