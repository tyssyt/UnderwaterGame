// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InventorySlot.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "XD/Resources/ResourceUI.h"
#include "InventorySlotUI.generated.h"

UCLASS(Abstract)
class XD_API UInventorySlotUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* Fill_1;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* Fill_2;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* Fill_3;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* Fill_4;

public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Current;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Max;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UResourceUI* Resource;

    UInventorySlotUI* Set(const FInventorySlot* inventorySlot);
    UInventorySlotUI* Set(int current, int max, UResource* resource);
    void Clear();

private:
    void SetFillPercent(float fill) const;

};
