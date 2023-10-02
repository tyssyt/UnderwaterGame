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

    UHotbar* CreateHotbar(
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
    ) const;

    UHotbarSlotSubmenu* CreateSubmenuSlot(UHotbar* submenu) const;
    UHotbarSlotAction* CreateActionSlot() const;
    UHotbarSlotAction* CreateBuilderModeSlot(UConstructionPlan* constructionPlan) const;
    UHotbarSlotAction* CreateIndoorBuilderModeSlot(UConstructionPlan* constructionPlan) const;

protected:

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UHotbar* MainHotbar;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* SubMenus;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UHotbar> HotbarUIClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UHotbarSlotSubmenu> HotbarSlotSubmenuUIClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UHotbarSlotAction> HotbarSlotActionUIClass;

    virtual void NativeOnInitialized() override;

public:

    void OpenSubmenu(UHotbar* submenu, const UHotbar* from);
    UHotbar* GetTop() const;
    void Reset() const;
    
};
