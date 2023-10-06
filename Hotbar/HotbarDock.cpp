// Fill out your copyright notice in the Description page of Project Settings.

#include "HotbarDock.h"

void UHotbarDock::OpenSubmenu(UHotbar* submenu, const UHotbar* from) const {
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

void UHotbarDock::SetMainHotbar(UHotbar* main) const {
    MainHotbar->Init(
        main->GetSlot(1),
        main->GetSlot(2),
        main->GetSlot(3),
        main->GetSlot(4),
        main->GetSlot(5),
        main->GetSlot(6),
        main->GetSlot(7),
        main->GetSlot(8),
        main->GetSlot(9),
        main->GetSlot(0)
    );
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
