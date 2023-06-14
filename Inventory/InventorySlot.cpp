// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlot.h"

FInventorySlot::FInventorySlot(): Max(0) {}
FInventorySlot::FInventorySlot(int max, const UResource* resource, AConveyor* AConveyor) : Max(max), Resource(resource), Conveyor(AConveyor) {}

FInventorySlot::~FInventorySlot() {}

int FInventorySlot::PushInto(int amount) {
    const int amountPushed = FMath::Min(amount, Max - Current);
    Current += amountPushed;
    return amountPushed;
}

int FInventorySlot::PullFrom(int amount) {
    const int amountPulled = FMath::Min(amount, Current);
    Current -= amountPulled;
    return amountPulled;
}
