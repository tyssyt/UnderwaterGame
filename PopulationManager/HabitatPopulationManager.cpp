// Fill out your copyright notice in the Description page of Project Settings.

#include "HabitatPopulationManager.h"

#include "Need.h"
#include "The.h"
#include "XD/Buildings/Habitat.h"
#include "XD/Buildings/IndoorBuilding.h"
#include "XD/Buildings/WorkerHouse.h"

UHabitatPopulationManager* UHabitatPopulationManager::Init(AHabitat* habitat) {
    Habitat = habitat;
    return this;
}

void UHabitatPopulationManager::TickPopulation() {
    // only Tick once a second
    if (++Tick < 60)
        return;
    Tick = 0;

    if (BuildingsChangedNotificationReceived)
        UpdateMaxPop();

    const int satisfiedPop = SatisfyNeeds();
    const bool popChanged = AdjustPop(satisfiedPop);

    if (popChanged || BuildingsChangedNotificationReceived)
        RelocatePop();
    BuildingsChangedNotificationReceived = false;

    ComputeWorkforce();

    /*
        what happens if there is not enough power for a city?
            I guess it has a power bank and runs on "emergency power" for a while?
            And a city without power does not consume goods, if people can they migrate to another dome
            City will not provide any workforce, research or whatever else it can
            And they just die at the 1% rate that everthing else uses
            propably want some form of prio system for energy (and workforce) so that cities or not "accidently" turned off...
            and propably also a warning if a substation or power producer is removed that would kill a cities power?
    */
}

void UHabitatPopulationManager::UpdateMaxPop() {
    int newMaxPop = 0;
    for (AIndoorBuilding* building : Habitat->Buildings) {
        if (AWorkerHouse* house = Cast<AWorkerHouse>(building)) {
            newMaxPop += AWorkerHouse::RESIDENT_LIMIT;
        }
    }

    if (newMaxPop != MaxPop) {
        MaxPop = newMaxPop;

        // TODO have a homeless grace period, OR at least show a warning before allowing the player to delete a building
        CurrentPop = FMath::Min(CurrentPop, newMaxPop);
    }
}

int UHabitatPopulationManager::SatisfyNeeds() {
    SatisfiedNeedsLastTick.Empty();
    const auto encyclopedia = The::Encyclopedia(Habitat);
    
    const int targetPop = FMath::Min(MaxPop, CurrentPop + ConsecutiveGrowthTicks * 25);
    int satisfiedPop = targetPop;

    const int biomePop = The::PopulationManager(this)->Population;
    for (const auto need : encyclopedia->GetAllNeeds()) {
        if (need->StartPop > biomePop)
            break;
        const int satisfiedPopBefore = satisfiedPop;
        satisfiedPop = need->IsSatisfied(Habitat, satisfiedPop);
        if (satisfiedPop == satisfiedPopBefore && targetPop > 0)
            SatisfiedNeedsLastTick.Add(need);

        if (satisfiedPopBefore > satisfiedPop) { // don't reduce under StartPop
            const float biomePopPercent = static_cast<float>(CurrentPop) / biomePop;
            const int adjustedStartPop = static_cast<int>(biomePopPercent * need->StartPop);
            satisfiedPop = FMath::Max(satisfiedPop, adjustedStartPop);
            satisfiedPop = FMath::Min(satisfiedPop, satisfiedPopBefore); // not sure if this is even possible... better safe than sorry
        }
    }

    if (satisfiedPop == targetPop && targetPop < MaxPop)
        ConsecutiveGrowthTicks++;
    else
        ConsecutiveGrowthTicks = 0;

    return satisfiedPop;
}

bool UHabitatPopulationManager::AdjustPop(int satisfiedPop) {   
    if (satisfiedPop < CurrentPop) {
        const int unsatisfiedPop = CurrentPop - satisfiedPop;
        CurrentPop = FMath::Max(satisfiedPop, CurrentPop - (unsatisfiedPop / 7) - 10);
        return true;
    }
    if (satisfiedPop > CurrentPop) {
        CurrentPop = satisfiedPop;
        return true;
    }
    return false;
}

void UHabitatPopulationManager::RelocatePop() const {
    int toRelocate = CurrentPop;
        
    // TODO order houses by comfort, consider caching by BuildingsChangedNotificationReceived
    for (AIndoorBuilding* building : Habitat->Buildings) {
        if (AWorkerHouse* house = Cast<AWorkerHouse>(building)) {
            const int livingHere = FMath::Min(toRelocate, AWorkerHouse::RESIDENT_LIMIT);
            house->Residents = livingHere;
            toRelocate -= livingHere;
        }
    }
    check(toRelocate == 0);
}

void UHabitatPopulationManager::ComputeWorkforce() {
    Workforce = CurrentPop; // TODO adjust for comfort/happiness
}
