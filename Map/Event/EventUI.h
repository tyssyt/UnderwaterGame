// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/RichTextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "EventUI.generated.h"

UCLASS(Abstract)
class XD_API UEventOptionUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY()
    UEventOption* EventOption;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* Button;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Text;

    UFUNCTION()
    void OnClick();

public:
    UEventOptionUI* Init(UEventOption* eventOption, bool enabled);
};

UCLASS(Abstract)
class XD_API UEventStageUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY()
    UEventStage* EventStage;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    URichTextBlock* Text;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Options;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UEventOptionUI> UEventOptionUIClass;

public:
    UEventStageUI* Init(UEventStage* eventStage, UEventOption* chosenOption);
    void Chose(UEventOption* option) const;
};

UCLASS(Abstract)
class XD_API UEventUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadOnly)
    UEventInstance* Event;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* EventName;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBorder* Background;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UScrollBox* Stages;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UEventStageUI> UEventStageUIClass;

public:
    UEventUI* Init(UEventInstance* event);
    void AddStage(UEventStage* eventStage, UEventOption* chosenOption) const;

    UFUNCTION(BlueprintCallable)
    void Open();
    UFUNCTION(BlueprintCallable)
    void Close();

    UEventInstance* GetEvent() const { return Event; }
};
