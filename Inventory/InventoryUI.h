// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventorySlotUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "InventoryUI.generated.h"

class UInventoryComponent;

UCLASS(Abstract)
class XD_API UInventoryUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Inputs;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Arrows;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Outputs;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UInventorySlotUI> InventorySlotUIClass;

    UPROPERTY()
    UInventoryComponent* Inventory;

public:
    UInventoryUI* Init(UInventoryComponent* inventory);
    void Tick() const;

};
