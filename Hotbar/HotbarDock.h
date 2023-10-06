// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hotbar.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "HotbarDock.generated.h"

class UConstructionPlan;
class UHotbarSlotAction;
class UHotbarSlotSubmenu;
class UHotbarSlot;

UCLASS(Abstract)
class XD_API UHotbarDock : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UHotbar* MainHotbar;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* SubMenus;

public:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UHotbar> HotbarUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UHotbarSlotSubmenu> HotbarSlotSubmenuUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UHotbarSlotAction> HotbarSlotActionUIClass;

    void OpenSubmenu(UHotbar* submenu, const UHotbar* from) const;
    void SetMainHotbar(UHotbar* main) const;
    UHotbar* GetTop() const;
    void Reset() const;
    
};
