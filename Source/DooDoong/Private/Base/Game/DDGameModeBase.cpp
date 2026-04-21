#include "Base/Game/DDGameModeBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "Base/Player/DDBasePlayerController.h"
#include "Base/Player/DDBasePlayerState.h"
#include "Common/DDLogManager.h"
#include "Data/DDUIConfig.h"
#include "System/DDGameplayTags.h"

ADDGameModeBase::ADDGameModeBase()
{
	PlayerStateClass = ADDBasePlayerState::StaticClass();
	PlayerControllerClass = ADDBasePlayerController::StaticClass();
	bUseSeamlessTravel = true;
}

void ADDGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ADDGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerController* ExitingPlayerController = Cast<APlayerController>(Exiting);
	if (IsValid(ExitingPlayerController))
	{
		AlivePlayerControllers.Remove(ExitingPlayerController);
	}
}

void ADDGameModeBase::HandleSeamlessTravelPlayer(AController*& ParticipantController)
{
	Super::HandleSeamlessTravelPlayer(ParticipantController);

	if (APawn* OldPawn = ParticipantController->GetPawn())
	{
		OldPawn->Destroy();
	}

	APlayerController* PlayerController = Cast<APlayerController>(ParticipantController);

	if (ADDBasePlayerState* BasePlayerState = ParticipantController->GetPlayerState<ADDBasePlayerState>())
	{
		LOG_CJH(Log, TEXT("[SeamlessTravel] 접속 유저: %s, 참여 여부: %d"),
		        *BasePlayerState->PlayerGameData.PlayerDisplayName.ToString(), BasePlayerState->bIsParticipant);

		if (!BasePlayerState->bIsParticipant)
		{
			SpawnSpectatorPawn(PlayerController);
			if (IsValid(PlayerController)) AlivePlayerControllers.Remove(PlayerController);

			return;
		}
	}

	RestartPlayer(ParticipantController);

	if (IsValid(PlayerController))
	{
		AlivePlayerControllers.AddUnique(PlayerController);
	}
	LOG_CJH(Log, TEXT("-> 참여자 스폰 완료! 현재 턴 명단 인원: %d명"), AlivePlayerControllers.Num());
}

void ADDGameModeBase::GenericPlayerInitialization(AController* C)
{
	Super::GenericPlayerInitialization(C);
	
	APlayerController* PlayerController = Cast<APlayerController>(C);
	if (IsValid(PlayerController))
	{
		AlivePlayerControllers.AddUnique(PlayerController);
		LOG_CJH(Log, TEXT("[GenericPlayerInitialization] 참여자 접속. 현재 접속 수: %d"), AlivePlayerControllers.Num());
	}
	
	if (CurrentUIConfig)
	{
		if (ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(C))
		{
			DDPC->Client_SetUIConfigByPath(FSoftObjectPath(CurrentUIConfig.Get())); 
		}
	}
}

void ADDGameModeBase::SetMatchState(FGameplayTag NewStateTag)
{
	// 자식 클래스에서 구현
}

void ADDGameModeBase::OnCharacterKilled(AActor* Killer, AActor* Victim)
{
	LOG_KMS(Warning, TEXT("[GM] : %s was killed by %s"), *Victim->GetName(), *Killer->GetName());
}

void ADDGameModeBase::HandleRespawn(AController* TargetController)
{
	if (TargetController)
	{
		// 1. 기존 캐릭터 제거
		if (APawn* OldPawn = TargetController->GetPawn())
		{
			OldPawn->Destroy();
		}

		// 2. ASC 초기화
		ADDBasePlayerState* PS = TargetController->GetPlayerState<ADDBasePlayerState>();
		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();

		ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(DDGameplayTags::State_Character_Death));
		ASC->RemoveLooseGameplayTag(DDGameplayTags::State_Character_Death);
		ASC->SetLooseGameplayTagCount(DDGameplayTags::State_Character_Death, 0);

		for (auto EffectClass : ReSpawnEffectClasses)
		{
			if (IsValid(EffectClass))
			{
				FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
				Context.AddSourceObject(ASC);
				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, 1.f, Context);
				if (SpecHandle.IsValid())
				{
					ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		}

		// 3. 새로운 캐릭터 생성 및 할당
		RestartPlayer(TargetController);
		LOG_KMS(Warning, TEXT("[GameMode] %s : ReSpawned."), *TargetController->GetPawn()->GetName());
	}
}

void ADDGameModeBase::FocusAllCamerasOnTarget(AActor* TargetActor)
{
	if (!IsValid(TargetActor)) return;

	for (APlayerController* PlayerController : AlivePlayerControllers)
	{
		if (IsValid(PlayerController))
		{
			PlayerController->SetViewTargetWithBlend(TargetActor, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
		}
	}
	LOG_CJH(Log, TEXT("[Camera] 모든 플레이어의 화면이 주인공 타겟(%s)을 향합니다."), *TargetActor->GetName());
}

UAbilitySystemComponent* ADDGameModeBase::GetAbilitySystemComponentFromPlayer(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return nullptr;

	APlayerState* PlayerState = PlayerController->PlayerState;
	if (!IsValid(PlayerState)) return nullptr;

	ADDBasePlayerState* BasePlayerState = Cast<ADDBasePlayerState>(PlayerState);
	if (!IsValid(BasePlayerState)) return nullptr;

	if (IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(BasePlayerState))
	{
		return AbilitySystemInterface->GetAbilitySystemComponent();
	}
	return nullptr;
}

void ADDGameModeBase::SpawnSpectatorPawn(APlayerController* PlayerController)
{
	if (PlayerController == nullptr || GetWorld() == nullptr)
	{
		return;
	}

	FVector SpectatorLocation = FVector(0.0f, 0.0f, 300.0f);
	FRotator SpectatorRotation = FRotator::ZeroRotator;

	AActor* StartSpot = AGameModeBase::ChoosePlayerStart_Implementation(PlayerController);
	if (StartSpot != nullptr)
	{
		SpectatorLocation = StartSpot->GetActorLocation() + FVector(0.0f, 0.0f, 200.0f);
		SpectatorRotation = StartSpot->GetActorRotation();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PlayerController;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (APawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(
		ASpectatorPawn::StaticClass(), SpectatorLocation, SpectatorRotation, SpawnParams))
	{
		PlayerController->Possess(SpectatorPawn);
	}
}

void ADDGameModeBase::BroadcastToAllPlayers(TFunctionRef<void(ADDBasePlayerController*)> Func)
{
	for (APlayerController* PC : AlivePlayerControllers)
	{
		if (ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(PC))
		{
			Func(DDPC);
		}
	}
}

void ADDGameModeBase::BroadcastGameLayer(const FGameplayTag NewGameTag)
{
	if (!NewGameTag.IsValid()) return;
	
	for (APlayerController* PC : AlivePlayerControllers)
	{
		ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(PC);
		if (IsValid(DDPC))
		{
			DDPC->Client_SwitchGameLayer(NewGameTag);
		}
	}
}

void ADDGameModeBase::BroadcastErrorMessage(const FString& ErrorMessage)
{
	LOG_KMS(Error, TEXT("%s"), *ErrorMessage);
	
	for (APlayerController* PC : AlivePlayerControllers)
	{
		ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(PC);
		if (IsValid(DDPC))
		{
			DDPC->Client_DrawErrorMessage(ErrorMessage);
		}
	}
}

void ADDGameModeBase::BroadcastOpenPopUp(FGameplayTag Tag)
{
	if (!Tag.IsValid()) return;
	
	for (APlayerController* PC : AlivePlayerControllers)
	{
		ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(PC);
		if (IsValid(DDPC))
		{
			
			DDPC->Client_OpenPopUp(Tag);
		}
	}
}

void ADDGameModeBase::BroadcastClosePopUp(FGameplayTag Tag)
{
	if (!Tag.IsValid()) return;
	
	for (APlayerController* PC : AlivePlayerControllers)
	{
		ADDBasePlayerController* DDPC = Cast<ADDBasePlayerController>(PC);
		if (IsValid(DDPC))
		{
			DDPC->Client_ClosePopUp(Tag);
		}
	}
}
