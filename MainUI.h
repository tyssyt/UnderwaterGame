// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotUI.h"
#include "MainUI.generated.h"

UCLASS(Abstract)
class XD_API UMainUI : public UUserWidget {
    GENERATED_BODY()
    
public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    URessourcesUI* WBP_RessourcesUI;
};

UCLASS(Abstract)
class XD_API URessourcesUI : public UUserWidget {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* InventorySlot_1;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* InventorySlot_2;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* InventorySlot_3;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* InventorySlot_4;

};
