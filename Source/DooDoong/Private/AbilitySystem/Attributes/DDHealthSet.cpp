#include "AbilitySystem/Attributes/DDHealthSet.h"

#include "GameplayEffectExtension.h"
#include "Base/Game/DDGameModeBase.h"
#include "Common/DDLogManager.h"
#include "Net/UnrealNetwork.h"
#include "System/DDGameplayTags.h"

UDDHealthSet::UDDHealthSet()
{
	InitHealth(30.0f);
	InitMaxHealth(30.0f);
}

void UDDHealthSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UDDHealthSet, Health);
	DOREPLIFETIME(UDDHealthSet, MaxHealth);
}

void UDDHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

void UDDHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	OnHealthChanged.Broadcast(GetHealth(), GetMaxHealth());
}

void UDDHealthSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// 1. Clamp 
		const float NewHealth = FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth());
		SetHealth(NewHealth);
		LOG_KMS(Warning, TEXT("Health Changed : %f"), NewHealth);
		
		// 2. 캐릭터 사망 체크 
		if (GetHealth() <= 0.f)
		{
			AActor* Killer = Data.EffectSpec.GetContext().GetInstigator();
			AActor* Victim = Data.Target.GetAvatarActor();
			
			// 3. 킬로그 브로드캐스트 
			KillLogSignature.Broadcast(Killer, Victim);
			if (ADDGameModeBase* GM = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode()))
			{
				GM->OnCharacterKilled(Killer, Victim);
			}
			
			// 4. 사망 이벤트 시작(GA_Death 호출) 
			UAbilitySystemComponent* TargetASC = &Data.Target; 
			if (TargetASC->AbilityActorInfo->IsNetAuthority())
			{
				FGameplayEventData Payload;
				Payload.Instigator = Data.EffectSpec.GetContext().GetInstigator();
				Payload.Target = Data.Target.GetAvatarActor();
				
				TargetASC->HandleGameplayEvent(DDGameplayTags::Event_Character_Death, &Payload);
				LOG_KMS(Warning, TEXT("Character Death %s"), *TargetASC->GetAvatarActor()->GetName());
			
				TargetASC->AddLooseGameplayTag(DDGameplayTags::State_Character_Death);
			}
		}
	}
	
}

void UDDHealthSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDDHealthSet, Health, OldHealth);
}

void UDDHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDDHealthSet, MaxHealth, OldMaxHealth);
}
