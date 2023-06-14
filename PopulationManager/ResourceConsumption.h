// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "XD/Inventory/InventorySlot.h"

class AHabitat;

class XD_API ResourceConsumption {
public:
    ResourceConsumption(int habitatInventoryIndex, int peoplePerResource);
    ~ResourceConsumption();

    // returns number of people who could not be given the resource, or 0 if all could be satisfied
    int ConsumeFrom(AHabitat* habitat, int population);
    
private:    
    const int HabitatInventoryIndex;
    const int PeoplePerResource;

    int Remainder; // I am too tired to evaluate this, but Remainder is a real word right? I am not going crazy?!?
    
    FInventorySlot* GetInventorySlot(AHabitat* habitat) const;
};
