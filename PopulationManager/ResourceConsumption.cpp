// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceConsumption.h"
#include "XD/Buildings/Habitat.h"

ResourceConsumption::ResourceConsumption(int peoplePerResource, UResource* const resource): PeoplePerResource(peoplePerResource), Resource(resource) {}

int ResourceConsumption::IsSatisfied(const AHabitat* habitat, int population) const {
    const auto slot = habitat->Inventory->GetInput(Resource);
    if (!slot)
        return 0;

    // TODO right now, the "remainder" is lost. It would be fun to maybe roll a weighted random number instead?
    const int wantedResources = (population + PeoplePerResource -1) / PeoplePerResource;
    const int consumedResources = slot->PullFrom(wantedResources);
    const int satisfiedPopulation = consumedResources * PeoplePerResource;
    return satisfiedPopulation; // this might be > population, caller needs to handle that
}
