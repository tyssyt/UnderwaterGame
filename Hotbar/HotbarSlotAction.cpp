// Fill out your copyright notice in the Description page of Project Settings.

#include "HotbarSlotAction.h"

void UHotbarSlotAction::Activate() {
    Action.Execute();
}

UImage* UHotbarSlotAction::GetImage() {
    return Image;
}
