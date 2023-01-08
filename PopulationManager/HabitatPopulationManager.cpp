// Fill out your copyright notice in the Description page of Project Settings.

#include "HabitatPopulationManager.h"

#include "XD/Buildings/Habitat.h"
#include "XD/Buildings/IndoorBuilding.h"
#include "XD/Buildings/WorkerHouse.h"

HabitatPopulationManager::HabitatPopulationManager(AHabitat* habitat)
    : Tick(0),
      BuildingsChangedNotificationRecieved(true),
      Habitat(habitat),
      MaxPop(0),
      SettledPop(0),
      StarvingPop(0),
      HomelessPop(0),
      Workforce(0),
      Hunger(ResourceConsumption(0, 100)) {}

HabitatPopulationManager::~HabitatPopulationManager() {}

void HabitatPopulationManager::NotifyBuildingsChanged() {
    BuildingsChangedNotificationRecieved = true;
}

void HabitatPopulationManager::TickPopulation() {
    // only Tick once a second
    if (++Tick < 60)
        return;
    Tick = 0;

    // update Max Pop
    if (BuildingsChangedNotificationRecieved) {
        UpdateMaxPop();
    }

    // consume resources, derive starving
    StarvingPop = Hunger.ConsumeFrom(Habitat, SettledPop);

    bool popChanged = GrowOrShrinkPop();

    // adjust residence counters in Houses
    if (popChanged || BuildingsChangedNotificationRecieved) {
        RelocatePop();
    }
    BuildingsChangedNotificationRecieved = false;

    Workforce = ComputeWorkforce();

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

void HabitatPopulationManager::UpdateMaxPop() {
    int newMaxPop = 0;
    for (AIndoorBuilding* building : Habitat->Buildings) {
        AWorkerHouse* house = Cast<AWorkerHouse>(building);
        if (house) {
            newMaxPop += AWorkerHouse::RESIDENT_LIMIT;
        }
    }

    // TODO adjust pop limit w.r.t. key buildings

    if (newMaxPop != MaxPop) {
        MaxPop = newMaxPop;

        // recalculate Homelessness
        const int totalPop = SettledPop + HomelessPop;
        HomelessPop = FMath::Max(0, totalPop - MaxPop);
        SettledPop -= HomelessPop;
    }
}

bool HabitatPopulationManager::GrowOrShrinkPop() {
    if (StarvingPop == 0 && HomelessPop == 0) {
        if (SettledPop < MaxPop) {
            // we can grow the population
            int increase = SettledPop / 100 + 3;
            increase = FMath::Min(MaxPop - SettledPop, increase);
            increase -= Hunger.ConsumeFrom(Habitat, increase);
            SettledPop += increase;
            return increase != 0;
        }
    } else {
        // there are starving / homeless people, so we shrink the population
        const int totalPop = SettledPop + HomelessPop;
        int reduction = totalPop / 100 + 3;

        // first reduce from homeless, then from starving
        // again, feels like math can do something that does not need an if here...
        if (HomelessPop >= reduction) {
            HomelessPop -= reduction;
        } else {
            reduction -= HomelessPop;
            HomelessPop = 0;
            reduction = FMath::Min(reduction, StarvingPop);
            SettledPop -= reduction;
            StarvingPop -= reduction;
        }
        return true;
    }
    return false;
}

int HabitatPopulationManager::ComputeWorkforce() const {
    if (SettledPop == 0 && HomelessPop == 0) {
        return 0;
    }
    
    // once we get comfort from buildings this gets more tricky because starving needs to be a global factor and not house specific
    const int unadjustedWorkforce = SettledPop; // TODO comfort calculation etc
    
    // adjust for starving
    const float percentStarving = static_cast<float>(StarvingPop) / SettledPop;
    const int adjustedWorkforce = static_cast<int>(unadjustedWorkforce * (1.f - .5f * percentStarving));
    
    const int homelessBonus = HomelessPop / 5;
    return adjustedWorkforce + homelessBonus;

    /*    
    next we calculate workforce
        consume comfort goods, no idea how the algorithm works if there is not enough of one?
            I guess each comfort good just adds to the comfort multiplier and if it usually adds 20% but only fullfilled for
        pop*comfort sum over all 
        reducing for starving people?
            percentStarving = starvingPeeps / allPeeps
            workforceAdjusted = (1-percentStarving)*workforce  + percentStarving*workforce/2 , i.e. 50% starving => 75% workforce
                or by math: worforce *= 1 - (0.5 * starvingPeeps / allPeeps)
        consider applying a starving debuff to the comfort of all houses...
    */
}

void HabitatPopulationManager::RelocatePop() const {
    int remaining = SettledPop;
        
    // TODO order houses by comfort, consider caching by BuildingsChangedNotificationRecieved
    for (AIndoorBuilding* building : Habitat->Buildings) {
        AWorkerHouse* house = Cast<AWorkerHouse>(building);
        if (house) {
            int livingHere = FMath::Min(remaining, AWorkerHouse::RESIDENT_LIMIT);
            house->Residents = livingHere;
            remaining -= livingHere;
        }
    }
}

int HabitatPopulationManager::GetMaxPop() const {
    return MaxPop;
}

int HabitatPopulationManager::GetSettledPop() const {
    return SettledPop;
}

int HabitatPopulationManager::GetFedPop() const {
    return SettledPop - StarvingPop;
}

int HabitatPopulationManager::GetStarvingPop() const {
    return StarvingPop;
}

int HabitatPopulationManager::GetHomelessPop() const {
    return HomelessPop;
}

int HabitatPopulationManager::GetWorkforce() const {
    return Workforce;
}
