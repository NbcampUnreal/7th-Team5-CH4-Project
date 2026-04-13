#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "AbilitySystemComponent.h"
#include "BasePlayerRowWidget.generated.h"

class UTextBlock;
class ADDBasePlayerState;
class UAbilitySystemComponent;
class UDDHealthSet;

UCLASS()
class DOODOONG_API UBasePlayerRowWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void Init(APlayerState* InPS);

protected:
    virtual void NativeConstruct() override;

    // =========================
    // UI
    // =========================

    UPROPERTY(meta = (BindWidget))
    UTextBlock* NameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* HealthText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* GoldText;

private:

    // =========================
    // 캐싱
    // =========================

    TWeakObjectPtr<ADDBasePlayerState> CachedPS;
    TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

    // =========================
    // GAS
    // =========================

    UFUNCTION()
    void OnGASReady();


    void OnHealthChanged(const FOnAttributeChangeData& Data);

    // =========================
    // UI 업데이트
    // =========================

    void UpdateHealth(float NewHealth);
    void UpdateGold(int32 NewGold);
};