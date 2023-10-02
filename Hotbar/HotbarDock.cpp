// Fill out your copyright notice in the Description page of Project Settings.

#include "HotbarDock.h"

#include "HotbarSlotAction.h"
#include "HotbarSlotSubmenu.h"
#include "XD/CameraPawn.h"
#include "XD/GameInstanceX.h"
#include "XD/Buildings/Conveyor.h"
#include "XD/Construction/BuilderModeExtension.h"
#include "XD/Construction/BuildingBuilderMode.h"
#include "XD/Construction/ConveyorBuilderMode.h"
#include "XD/Construction/IndoorBuilderMode.h"

UHotbar* UHotbarDock::CreateHotbar(
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
) const {
    return CreateWidget<UHotbar>(GetOwningPlayer(), HotbarUIClass)->Init(slot1, slot2, slot3, slot4, slot5, slot6, slot7, slot8, slot9, slot0);
}

UHotbarSlotSubmenu* UHotbarDock::CreateSubmenuSlot(UHotbar* submenu) const {
    return CreateWidget<UHotbarSlotSubmenu>(GetOwningPlayer(), HotbarSlotSubmenuUIClass)->Init(submenu);
}

UHotbarSlotAction* UHotbarDock::CreateActionSlot() const {
    return CreateWidget<UHotbarSlotAction>(GetOwningPlayer(), HotbarSlotActionUIClass);
}

UHotbarSlotAction* UHotbarDock::CreateBuilderModeSlot(UConstructionPlan* constructionPlan) const {
    const auto pawn = GetPlayerContext().GetPawn<ACameraPawn>();
    return CreateActionSlot()->Init(constructionPlan->Image, [constructionPlan, pawn]{
        if (pawn->PrepBuilderMode(constructionPlan->BuildingClass))
            pawn->BuilderMode = NewObject<UBuildingBuilderMode>(pawn)->Init(constructionPlan, nullptr);
    });
}

UHotbarSlotAction* UHotbarDock::CreateIndoorBuilderModeSlot(UConstructionPlan* constructionPlan) const {
    const auto pawn = GetPlayerContext().GetPawn<ACameraPawn>();
    return CreateActionSlot()->Init(constructionPlan->Image, [constructionPlan, pawn]{
        if (pawn->PrepBuilderMode(constructionPlan->BuildingClass))
            pawn->BuilderMode = NewObject<UIndoorBuilderMode>(pawn)->Init(constructionPlan, nullptr);
    });
}

void UHotbarDock::NativeOnInitialized() {
    Super::NativeOnInitialized();

    const auto pawn = GetPlayerContext().GetPawn<ACameraPawn>();
    const auto theBuildingBook = GetGameInstance<UGameInstanceX>()->TheBuildingBook;

    // TODO remove
    const auto dummyBar1 = CreateHotbar(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    const auto dummyBar2 = CreateHotbar(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    const auto dummyBar3 = CreateHotbar(CreateSubmenuSlot(dummyBar2), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

    const auto logisticsBar = CreateHotbar(
        CreateActionSlot()->Init(theBuildingBook->ConveyorLink->Image, [pawn]{
            if (pawn->PrepBuilderMode(AConveyor::StaticClass()))
                pawn->BuilderMode = NewObject<UConveyorBuilderMode>(pawn)->Init();            
        }),
        CreateBuilderModeSlot(theBuildingBook->Depot),
        CreateBuilderModeSlot(theBuildingBook->PickupPad),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        CreateSubmenuSlot(dummyBar1),
        CreateSubmenuSlot(dummyBar3)
    );

    const auto industryBar = CreateHotbar(
        CreateBuilderModeSlot(theBuildingBook->Smelter),
        CreateBuilderModeSlot(theBuildingBook->AssemblyLine),
        CreateBuilderModeSlot(theBuildingBook->Excavator),
        nullptr, // farm
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );

    const auto powerBar = CreateHotbar(
        CreateActionSlot()->Init(theBuildingBook->Substation->Image, [pawn, theBuildingBook]{
            if (pawn->PrepBuilderMode(ASubstation::StaticClass()))
                pawn->BuilderMode = NewObject<UBuildingBuilderMode>(pawn)->Init(theBuildingBook->Substation, NewObject<USubstationBuilderModeExtension>(pawn));            
        }),
        CreateBuilderModeSlot(theBuildingBook->Solar),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
        );

    const auto habitatBar = CreateHotbar(
        CreateBuilderModeSlot(theBuildingBook->Habitat),
        CreateIndoorBuilderModeSlot(theBuildingBook->WorkerHouse),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );

    MainHotbar->Init(
        CreateSubmenuSlot(logisticsBar),
        CreateSubmenuSlot(industryBar),
        CreateSubmenuSlot(powerBar),
        CreateSubmenuSlot(habitatBar),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr // destroy Mode
    );
}

void UHotbarDock::OpenSubmenu(UHotbar* submenu, const UHotbar* from) {
    if (from == MainHotbar) {
        Reset();
        SubMenus->AddChildToVerticalBox(submenu);
    } else {
        const int idx = SubMenus->GetChildIndex(from);
        check(idx != INDEX_NONE);
        for (int i = 0; i <= idx; ++i)
            Cast<UHotbar>(SubMenus->GetChildAt(i))->Reset();

        TArray<UWidget*> newSubMenus;
        newSubMenus.Add(submenu);
        for (int i = idx; i < SubMenus->GetChildrenCount(); ++i)
            newSubMenus.Add(SubMenus->GetChildAt(i));

        SubMenus->ClearChildren();
        for (auto sm : newSubMenus)
            SubMenus->AddChildToVerticalBox(sm);
    }
}

UHotbar* UHotbarDock::GetTop() const {
    return SubMenus->HasAnyChildren() ? Cast<UHotbar>(SubMenus->GetChildAt(0)) : MainHotbar;
}

void UHotbarDock::Reset() const {
    for (const auto child : SubMenus->GetSlots())
        Cast<UHotbar>(child->Content)->Reset();
    SubMenus->ClearChildren();
    MainHotbar->Reset();
}
