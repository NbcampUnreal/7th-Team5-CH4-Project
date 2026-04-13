#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "System/MiniGame/DDMiniGameDefinition.h"
#include "Components/TextBlock.h"
#include "DDMiniGameReadyWidget.generated.h"



class UTextBlock;

UCLASS()
class DOODOONG_API UDDMiniGameReadyWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void HandleReadyStateChanged(int32 Ready, int32 Total);

    void BindToGameState();

    const UDDMiniGameDefinition* FallbackDefinition = nullptr;

    UFUNCTION(BlueprintCallable)
    void SetMiniGameData(const UDDMiniGameDefinition* Definition);

    FString GetDefinitionPathByID(FName ID) const;


    UFUNCTION(BlueprintImplementableEvent)
    void UpdateReadyCount(int32 Ready, int32 Total);

    UFUNCTION(BlueprintCallable)
    void SetReadyCount(int32 Ready, int32 Total);

protected:
    void ApplyToUI();

protected:
    UPROPERTY(BlueprintReadOnly, Category = "MiniGame")
    FText GameName;

    UPROPERTY(BlueprintReadOnly, Category = "MiniGame")
    FText Description;

    UPROPERTY(BlueprintReadOnly, Category = "MiniGame")
    float TimeLimit;

    UPROPERTY(BlueprintReadOnly, Category = "MiniGame")
    FGameplayTag Genre;





    UPROPERTY(meta = (BindWidget))
    UTextBlock* ReadyText;

    // ===== UMG 바인딩 =====
    UPROPERTY(meta = (BindWidget))
    UTextBlock* GameNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DescriptionText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimeLimitText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* GenreText;
};