#include "UI/Widgets/BasePlayerRowWidget.h"
#include "Components/TextBlock.h"
#include "Base/Player/DDBasePlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/DDHealthSet.h"
#include "GameFramework/PlayerState.h"

void UBasePlayerRowWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Warning, TEXT("PlayerRow Constructed"));
}

void UBasePlayerRowWidget::Init(APlayerState* InPS)
{
    ADDBasePlayerState* PS = Cast<ADDBasePlayerState>(InPS);
    if (!PS) return;

    CachedPS = PS;

    // 이름 먼저 표시
    if (NameText)
    {
        NameText->SetText(FText::FromString(PS->GetPlayerName()));
    }

    // 기본값 (GAS 오기 전)
    //  초기 Health 직접 읽기
    if (const UDDHealthSet* HealthSet = PS->GetHealthSet())
    {
        float Health = HealthSet->GetHealth();
        UpdateHealth(Health);

        UE_LOG(LogTemp, Warning, TEXT("[Row] Init Health = %f"), Health);
    }


    UpdateGold(0);

    // 🔥 GAS 준비 이벤트 구독
    PS->OnGASReady.AddUObject(this, &UBasePlayerRowWidget::OnGASReady);
}



void UBasePlayerRowWidget::OnGASReady()
{
    if (!CachedPS.IsValid()) return;

    UAbilitySystemComponent* ASC = CachedPS->GetAbilitySystemComponent();
    if (!ASC) return;

    CachedASC = ASC;

    const UDDHealthSet* HealthSet = CachedPS->GetHealthSet();
    if (!HealthSet) return;

   

    // 🔥 변화 감지
    ASC->GetGameplayAttributeValueChangeDelegate(
        HealthSet->GetHealthAttribute()
    ).AddUObject(this, &UBasePlayerRowWidget::OnHealthChanged);
}



void UBasePlayerRowWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    UpdateHealth(Data.NewValue);
}







void UBasePlayerRowWidget::UpdateHealth(float NewHealth)
{
    if (HealthText)
    {
        HealthText->SetText(
            FText::FromString(
                FString::Printf(TEXT("HP: %.0f"), NewHealth)
            )
        );
    }
}

void UBasePlayerRowWidget::UpdateGold(int32 NewGold)
{
    if (GoldText)
    {
        GoldText->SetText(
            FText::FromString(
                FString::Printf(TEXT("Gold: %d"), NewGold)
            )
        );
    }
}