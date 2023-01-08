// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceConsumption.h"
#include "XD/Buildings/Habitat.h"

ResourceConsumption::ResourceConsumption(int habitatInventoryIndex, int peoplePerResource): HabitatInventoryIndex(habitatInventoryIndex), PeoplePerResource(peoplePerResource), Remainder(0) {}

ResourceConsumption::~ResourceConsumption() {}

int ResourceConsumption::ConsumeFrom(AHabitat* habitat, int population) {
    if (Remainder >= population) {
        Remainder -= population;
        return 0;
    }

    // first, use the Remainder
    population -= Remainder;

    const int wantedResources = (population + PeoplePerResource -1) / PeoplePerResource;
    const int consumedResources = GetInventorySlot(habitat)->PullFrom(wantedResources);

    const int satisfiedPopulation = consumedResources * PeoplePerResource;

    // we could do this as math min and math max statement, think about which one is more readable
    if (satisfiedPopulation > population) {
        Remainder = satisfiedPopulation - population;
        return 0;
    } else {
        Remainder = 0;
        return population - satisfiedPopulation;
    }
}

FInventorySlot* ResourceConsumption::GetInventorySlot(AHabitat* habitat) const {
    return &habitat->Inventory->GetInputs()[HabitatInventoryIndex];
}
