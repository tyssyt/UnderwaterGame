// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Hotbar.generated.h"

class UHotbarDock;
class UHotbarSlot;

UCLASS(Abstract)
class XD_API UHotbar : public UUserWidget {
    GENERATED_BODY()

public:
    UHotbar* Init(
        UHotbarSlot* slot1,
        UHotbarSlot* slot2,
        UHotbarSlot* slot3,
        UHotbarSlot* slot4,
        UHotbarSlot* slot5,
        UHotbarSlot* slot6,
        UHotbarSlot* slot7,
        UHotbarSlot* slot8,
        UHotbarSlot* slot9,
        UHotbarSlot* slot0
    );

    UHotbarDock* GetDock();
    UFUNCTION(BlueprintCallable)
    UHotbarSlot* GetSlot(int i) const;

    void Reset() const;

protected:

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* Button_1;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UNamedSlot* NamedSlot_1;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* Button_2;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UNamedSlot* NamedSlot_2;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* Button_3;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UNamedSlot* NamedSlot_3;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* Button_4;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UNamedSlot* NamedSlot_4;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* Button_5;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UNamedSlot* NamedSlot_5;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* Button_6;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UNamedSlot* NamedSlot_6;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* Button_7;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UNamedSlot* NamedSlot_7;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* Button_8;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UNamedSlot* NamedSlot_8;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* Button_9;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UNamedSlot* NamedSlot_9;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* Button_0;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UNamedSlot* NamedSlot_0;

};
