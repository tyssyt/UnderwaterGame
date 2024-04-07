// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "XD/Inventory/InventorySlot.h"

class AHabitat;

class XD_API ResourceConsumption {
public:
    const int PeoplePerResource;
    UResource* const Resource;

    ResourceConsumption(int peoplePerResource, UResource* const resource);
    int IsSatisfied(const AHabitat* habitat, int population) const;
};
