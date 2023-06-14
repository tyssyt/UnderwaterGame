// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingBook.h"

#include "ConductiveOreMine.h"
#include "Depot.h"
#include "Habitat.h"
#include "HardOreMine.h"
#include "OilFishHarvester.h"
#include "PickupPad.h"
#include "Smelter.h"
#include "Solar.h"
#include "Splitter.h"
#include "Substation.h"
#include "WorkerHouse.h"

UBuildingBook* UBuildingBook::Init(const UResourceBook* resourceBook) {
    // TODO fix Images
    // TODO the UI does not use the name string here but has a redundant copy. Remove the redundency because at some point someone (me) will forget to update both!
    
    ConductiveOreMine = NewObject<UConstructionPlan>(this)->Init(AConductiveOreMine::StaticClass(), FText::FromString(TEXT("Conductive Ore Mine")), TEXT("/Game/Assets/Resources/Unpowered"), 1, {Material(10, resourceBook->LargeParts)});
    Depot = NewObject<UConstructionPlan>(this)->Init(ADepot::StaticClass(), FText::FromString(TEXT("Depot")), TEXT("/Game/Assets/Resources/Unpowered"), 1, {Material(10, resourceBook->LargeParts)});
    Habitat = NewObject<UConstructionPlan>(this)->Init(AHabitat::StaticClass(), FText::FromString(TEXT("Habitat")), TEXT("/Game/Assets/Resources/Unpowered"), 1, {Material(10, resourceBook->LargeParts)});
    HardOreMine = NewObject<UConstructionPlan>(this)->Init(AHardOreMine::StaticClass(), FText::FromString(TEXT("Hard Ore Mine")), TEXT("/Game/Assets/Resources/Unpowered"), 1, {Material(10, resourceBook->LargeParts)});
    OilFishHarvester = NewObject<UConstructionPlan>(this)->Init(AOilFishHarvester::StaticClass(), FText::FromString(TEXT("Oil Fish Harvester")), TEXT("/Game/Assets/Resources/Unpowered"), 1, {Material(10, resourceBook->LargeParts)});
    PickupPad = NewObject<UConstructionPlan>(this)->Init(APickupPad::StaticClass(), FText::FromString(TEXT("Pickup Pad")), TEXT("/Game/Assets/Resources/Unpowered"), 1, {Material(10, resourceBook->LargeParts)});
    Smelter = NewObject<UConstructionPlan>(this)->Init(ASmelter::StaticClass(), FText::FromString(TEXT("Smelter")), TEXT("/Game/Assets/Resources/Unpowered"), 1, {Material(10, resourceBook->LargeParts)});
    Solar = NewObject<UConstructionPlan>(this)->Init(ASolar::StaticClass(), FText::FromString(TEXT("Solar")), TEXT("/Game/Assets/Resources/Unpowered"), 1, {Material(10, resourceBook->LargeParts)});
    Substation = NewObject<UConstructionPlan>(this)->Init(ASubstation::StaticClass(), FText::FromString(TEXT("Substation")), TEXT("/Game/Assets/Resources/Unpowered"), 1, {Material(10, resourceBook->LargeParts), Material(10, resourceBook->Cable)});
    WorkerHouse = NewObject<UConstructionPlan>(this)->Init(AWorkerHouse::StaticClass(), FText::FromString(TEXT("Worker House")), TEXT("/Game/Assets/Resources/Unpowered"), 1, {Material(10, resourceBook->LargeParts)});
    Splitter = NewObject<UConstructionPlan>(this)->Init(ASplitter::StaticClass(), FText::FromString(TEXT("Splitter")), TEXT("/Game/Assets/Resources/Unpowered"), 1, {Material(10, resourceBook->LargeParts)});
    Merger = NewObject<UConstructionPlan>(this)->Init(AMerger::StaticClass(), FText::FromString(TEXT("Merger")), TEXT("/Game/Assets/Resources/Unpowered"), 1, {Material(10, resourceBook->LargeParts)});
    
    return this;
}
