// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HotbarSlot.h"
#include "Components/Image.h"
#include "HotbarSlotSubmenu.generated.h"

class UHotbar;

UCLASS(Abstract)
class XD_API UHotbarSlotSubmenu : public UHotbarSlot {
    GENERATED_BODY()

    UPROPERTY()
    UHotbar* Submenu;

protected:

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* Image_1;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* Image_2;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* Image_3;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* Image_4;

public:

    UHotbarSlotSubmenu* Init(UHotbar* submenu);
    virtual void Activate() override;
    virtual UImage* GetImage() override;
};
