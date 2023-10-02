// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "HotbarSlot.generated.h"

class UHotbarDock;
class UHotbar;

UCLASS(Abstract)
class XD_API UHotbarSlot : public UUserWidget {
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual void Activate() {}
    virtual UImage* GetImage() {return nullptr;}

    UHotbar* GetHotbar();
    UHotbarDock* GetDock();
};
