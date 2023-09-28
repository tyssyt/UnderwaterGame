// Fill out your copyright notice in the Description page of Project Settings.

#include "BlueprintHolder.h"

void UBlueprintHolder::Init(APlayerController* controller) {
    Super::PostInitProperties();

    MainUI = CreateWidget<UMainUI>(controller, MainUIClass);
    HardOreDepositUI = CreateWidget<UHardOreDepositUI>(controller, HardOreDepositUIClass);
    ConductiveOreDepositUI = CreateWidget<UConductiveOreDepositUI>(controller, ConductiveOreDepositUIClass);
    OilFishUI = CreateWidget<UOilFishUI>(controller, OilFishUIClass);
    HardOreMineUI = CreateWidget<UHardOreMineUI>(controller, HardOreMineUIClass);
    ConductiveOreMineUI = CreateWidget<UConductiveOreMineUI>(controller, ConductiveOreMineUIClass);
    OilFishHarvesterUI = CreateWidget<UOilFishHarvesterUI>(controller, OilFishHarvesterUIClass);
    DepotUI = CreateWidget<UDepotUI>(controller, DepotUIClass);
    ConveyorUI = CreateWidget<UConveyorUI>(controller, ConveyorUIClass);
    SmelterUI = CreateWidget<USmelterUI>(controller, SmelterUIClass);
    HabitatUI = CreateWidget<UHabitatUI>(controller, HabitatUIClass);
    WorkerHouseUI = CreateWidget<UWorkerHouseUI>(controller, WorkerHouseUIClass);
    PickupPadUI = CreateWidget<UPickupPadUI>(controller, PickupPadUIClass);
    AssemblyLineUI = CreateWidget<UAssemblyLineUI>(controller, AssemblyLineUIClass);
    SubstationUI = CreateWidget<USubstationUI>(controller, SubstationUIClass);
    ExcavatorUI = CreateWidget<UExcavatorUI>(controller, ExcavatorUIClass);
    PowerOverlayUI = CreateWidget<UPowerOverlayUI>(controller, PowerOverlayUIClass);
    ConstructionUI = CreateWidget<UConstructionUI>(controller, ConstructionUIClass);
}

USelectedUI* UBlueprintHolder::GetUI(AXActor* actor) const {
    if (const auto building = Cast<ABuilding>(actor))
        if (building->constructionState != EConstructionState::Done)
            return nullptr; // TODO construction site UI
    
    if (actor->IsA(AHardOreDeposit::StaticClass())) {
        HardOreDepositUI->Deposit = Cast<AHardOreDeposit>(actor);
        return HardOreDepositUI;
    }
    if (actor->IsA(AConductiveOreDeposit::StaticClass())) {
        ConductiveOreDepositUI->Deposit = Cast<AConductiveOreDeposit>(actor);
        return ConductiveOreDepositUI;
    }
    if (actor->IsA(AOilFish::StaticClass())) {
        OilFishUI->Deposit = Cast<AOilFish>(actor);
        return OilFishUI;
    }
    if (actor->IsA(AHardOreMine::StaticClass())) {
        HardOreMineUI->Mine = Cast<AHardOreMine>(actor);
        return HardOreMineUI;
    }
    if (actor->IsA(AConductiveOreMine::StaticClass())) {
        ConductiveOreMineUI->Mine = Cast<AConductiveOreMine>(actor);
        return ConductiveOreMineUI;
    }
    if (actor->IsA(AOilFishHarvester::StaticClass())) {
        OilFishHarvesterUI->Mine = Cast<AOilFishHarvester>(actor);
        return OilFishHarvesterUI;
    }
    if (actor->IsA(ADepot::StaticClass())) {
        DepotUI->Depot = Cast<ADepot>(actor);
        return DepotUI;
    }
    if (actor->IsA(AConveyor::StaticClass())) {
        ConveyorUI->Conveyor = Cast<AConveyor>(actor);
        return ConveyorUI;
    }
    if (actor->IsA(ASmelter::StaticClass())) {
        SmelterUI->Smelter = Cast<ASmelter>(actor);
        return SmelterUI;
    }
    if (actor->IsA(AHabitat::StaticClass())) {
        HabitatUI->Habitat = Cast<AHabitat>(actor);
        return HabitatUI;
    }
    if (actor->IsA(AWorkerHouse::StaticClass())) {
        WorkerHouseUI->House = Cast<AWorkerHouse>(actor);
        return WorkerHouseUI;
    }
    if (actor->IsA(APickupPad::StaticClass())) {
        PickupPadUI->PickupPad = Cast<APickupPad>(actor);
        return PickupPadUI;
    }
    if (actor->IsA(AAssemblyLine::StaticClass())) {
        AssemblyLineUI->AssemblyLine = Cast<AAssemblyLine>(actor);
        return AssemblyLineUI;
    }
    if (actor->IsA(ASubstation::StaticClass())) {
        SubstationUI->Substation = Cast<ASubstation>(actor);
        return SubstationUI;
    }
    if (actor->IsA(AExcavator::StaticClass())) {
        ExcavatorUI->Excavator = Cast<AExcavator>(actor);
        return ExcavatorUI;
    }
    return nullptr;
}
