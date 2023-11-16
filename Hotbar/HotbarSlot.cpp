// Fill out your copyright notice in the Description page of Project Settings.

#include "HotbarSlot.h"

#include "Hotbar.h"
#include "UI.h"

UHotbar* UHotbarSlot::GetHotbar() {
    return UX::GetParentWidget<UHotbar>(this);
}

UHotbarDock* UHotbarSlot::GetDock() {
    return UX::GetParentWidget<UHotbarDock>(this);
}
