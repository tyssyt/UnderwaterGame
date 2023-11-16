// Fill out your copyright notice in the Description page of Project Settings.

#include "BlueprintHolder.h"

#include "The.h"

void UBlueprintHolder::Init(APlayerController* controller) {
    Super::PostInitProperties();

    MainUI = CreateWidget<UMainUI>(controller, MainUIClass);
    EncyclopediaUI = CreateWidget<UEncyclopediaUI>(controller, EncyclopediaUIClass);
    ConveyorUI = CreateWidget<UConveyorUI>(controller, ConveyorUIClass);
    HabitatUI = CreateWidget<UHabitatUI>(controller, HabitatUIClass);
    WorkerHouseUI = CreateWidget<UWorkerHouseUI>(controller, WorkerHouseUIClass);
    PowerOverlayUI = CreateWidget<UPowerOverlayUI>(controller, PowerOverlayUIClass);
}

USelectedUI* UBlueprintHolder::GetUI(AXActor* actor) const {
    if (const auto building = Cast<ABuilding>(actor))
        if (building->constructionState != EConstructionState::Done)
            return nullptr; // TODO construction site UI

    // TODO these 3 are still special cases from the old way of doing it, they should be removed eventually
    if (actor->IsA(AConveyor::StaticClass())) {
        ConveyorUI->Conveyor = Cast<AConveyor>(actor);
        return ConveyorUI;
    }
    if (actor->IsA(AHabitat::StaticClass())) {
        HabitatUI->Habitat = Cast<AHabitat>(actor);
        return HabitatUI;
    }
    if (actor->IsA(AWorkerHouse::StaticClass())) {
        WorkerHouseUI->House = Cast<AWorkerHouse>(actor);
        return WorkerHouseUI;
    }

    const auto controller = The::PlayerController(actor);

    if (const auto buildingActor = Cast<ABuilding>(actor))
        return CreateWidget<UBuildingSelectedUI>(controller, BuildingUIClass)->Init(buildingActor);

    if (const auto naturalResourceActor = Cast<ANaturalResourceActor>(actor))
        return CreateWidget<UNaturalResourceSelectedUI>(controller, NaturalResourceUIClass)->Init(naturalResourceActor);

    return nullptr;
}
