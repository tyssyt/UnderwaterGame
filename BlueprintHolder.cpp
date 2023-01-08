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
}

USelectedUI* UBlueprintHolder::GetUI(AActor* actor) const {
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
    return nullptr;
}
