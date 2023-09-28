// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingBook.h"

#include "AssemblyLine.h"
#include "ConductiveOreMine.h"
#include "Depot.h"
#include "Excavator.h"
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

    ConductiveOreMine = NewObject<UConstructionPlan>(this)->Init(
        AConductiveOreMine::StaticClass(),
        FText::FromString(TEXT("Conductive Ore Mine")),
        TEXT("/Game/Assets/Resources/Placeholder"),
        1,
        {
            Material(200, resourceBook->LargeParts),
            Material(10, resourceBook->Cable),
            Material(100, resourceBook->ElectricalComponents),
            Material(200, resourceBook->MechanicalComponents),
            Material(100, resourceBook->LifeSupportEquipment)
        }
    );
    Depot = NewObject<UConstructionPlan>(this)->Init(
        ADepot::StaticClass(),
        FText::FromString(TEXT("Depot")),
        TEXT("/Game/Assets/Meshes/Previews/Depot"),
        1,
        {Material(50, resourceBook->LargeParts)}
    );
    Habitat = NewObject<UConstructionPlan>(this)->Init(
        AHabitat::StaticClass(),
        FText::FromString(TEXT("Habitat")),
        TEXT("/Game/Assets/Resources/Placeholder"),
        1,
        {
            Material(2000, resourceBook->LargeParts),
            Material(1000, resourceBook->LifeSupportEquipment),
            Material(1000, resourceBook->Glass),
            Material(200, resourceBook->Cable),
            Material(200, resourceBook->ElectricalComponents)
        }
    );
    HardOreMine = NewObject<UConstructionPlan>(this)->Init(
        AHardOreMine::StaticClass(),
        FText::FromString(TEXT("Hard Ore Mine")),
        TEXT("/Game/Assets/Resources/Placeholder"),
        1,
        {
            Material(200, resourceBook->LargeParts),
            Material(10, resourceBook->Cable),
            Material(100, resourceBook->ElectricalComponents),
            Material(200, resourceBook->MechanicalComponents),
            Material(100, resourceBook->LifeSupportEquipment)
        }
    );
    OilFishHarvester = NewObject<UConstructionPlan>(this)->Init(
        AOilFishHarvester::StaticClass(),
        FText::FromString(TEXT("Oil Fish Harvester")),
        TEXT("/Game/Assets/Resources/Placeholder"),
        1,
        {
            Material(100, resourceBook->LargeParts),
            Material(10, resourceBook->Cable),
            Material(100, resourceBook->ElectricalComponents),
            Material(100, resourceBook->LifeSupportEquipment)
        }
    );
    PickupPad = NewObject<UConstructionPlan>(this)->Init(
        APickupPad::StaticClass(),
        FText::FromString(TEXT("Pickup Pad")),
        TEXT("/Game/Assets/Meshes/Previews/PickupPad"),
        1,
        {Material(100, resourceBook->LargeParts)}
    );
    Smelter = NewObject<UConstructionPlan>(this)->Init(
        ASmelter::StaticClass(),
        FText::FromString(TEXT("Smelter")),
        TEXT("/Game/Assets/Resources/Placeholder"),
        1,
        {
            Material(100, resourceBook->LargeParts),
            Material(10, resourceBook->Cable),
            Material(200, resourceBook->ElectricalComponents),
            Material(50, resourceBook->LifeSupportEquipment)
        }
    );
    Solar = NewObject<UConstructionPlan>(this)->Init(
        ASolar::StaticClass(),
        FText::FromString(TEXT("Solar")),
        TEXT("/Game/Assets/Meshes/Previews/Solar"),
        1,
        {
            Material(100, resourceBook->Glass),
            Material(50, resourceBook->Cable),
            Material(100, resourceBook->ElectricalComponents)
        }
    );
    Substation = NewObject<UConstructionPlan>(this)->Init(
        ASubstation::StaticClass(),
        FText::FromString(TEXT("Substation")),
        TEXT("/Game/Assets/Meshes/Previews/Substation"),
        1,
        {
            Material(50, resourceBook->LargeParts),
            Material(50, resourceBook->Cable),
            Material(100, resourceBook->ElectricalComponents)
        }
    );
    WorkerHouse = NewObject<UConstructionPlan>(this)->Init(
        AWorkerHouse::StaticClass(),
        FText::FromString(TEXT("Worker House")),
        TEXT("/Game/Assets/Resources/Placeholder"),
        1,
        {Material(50, resourceBook->LargeParts)}
    );
    Splitter = NewObject<UConstructionPlan>(this)->Init(
        ASplitter::StaticClass(),
        FText::FromString(TEXT("Splitter")),
        TEXT("/Game/Assets/Resources/Placeholder"),
        1,
        {
            Material(25, resourceBook->LargeParts),
            Material(50, resourceBook->MechanicalComponents),
        }
    );
    Merger = NewObject<UConstructionPlan>(this)->Init(
        AMerger::StaticClass(),
        FText::FromString(TEXT("Merger")),
        TEXT("/Game/Assets/Resources/Placeholder"),
        1,
        {
            Material(25, resourceBook->LargeParts),
            Material(50, resourceBook->MechanicalComponents),
        }
    );
    AssemblyLine = NewObject<UConstructionPlan>(this)->Init(
        AAssemblyLine::StaticClass(),
        FText::FromString(TEXT("Assembly Line")),
        TEXT("/Game/Assets/Meshes/Previews/AssemblyLine"),
        1,
        {
            Material(200, resourceBook->LargeParts),
            Material(10, resourceBook->Cable),
            Material(100, resourceBook->ElectricalComponents),
            Material(100, resourceBook->MechanicalComponents),
            Material(50, resourceBook->LifeSupportEquipment)
        }
        );
    Excavator = NewObject<UConstructionPlan>(this)->Init(
        AExcavator::StaticClass(),
        FText::FromString(TEXT("Excavator")),
        TEXT("/Game/Assets/Meshes/Previews/Excavator"),
        1,
        {
            Material(200, resourceBook->LargeParts),
            Material(10, resourceBook->Cable),
            Material(50, resourceBook->ElectricalComponents),
            Material(100, resourceBook->MechanicalComponents),
            Material(50, resourceBook->LifeSupportEquipment)
        }
    );

    ConveyorNode = NewObject<UConstructionPlan>(this)->Init(
        AConveyor::StaticClass(),
        FText::FromString(TEXT("Conveyor Node")),
        TEXT("/Game/Assets/Resources/Placeholder"),
        1,
        {
            Material(15, resourceBook->LargeParts)
        }
    );
    ConveyorLink = NewObject<UConstructionPlan>(this)->Init(
        AConveyor::StaticClass(),
        FText::FromString(TEXT("Conveyor Link")),
        TEXT("/Game/Assets/Resources/Placeholder"),
        1,
        {
            Material(2, resourceBook->LargeParts),
            Material(1, resourceBook->Rubber)
        }
    );

    return this;
}
