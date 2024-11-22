// Fill out your copyright notice in the Description page of Project Settings.

#include "BlueprintHolder.h"

#include "The.h"

void UBlueprintHolder::Init(APlayerController* controller) {
    Super::PostInitProperties();

    MainUI = CreateWidget<UMainUI>(controller, MainUIClass);
    EncyclopediaUI = CreateWidget<UEncyclopediaUI>(controller, EncyclopediaUIClass);
    MapUI = CreateWidget<UMapUI>(controller, MapUIClass);
    ConveyorUI = CreateWidget<UConveyorUI>(controller, ConveyorUIClass);
    PowerOverlayUI = CreateWidget<UPowerOverlayUI>(controller, PowerOverlayUIClass);
}

USelectedUI* UBlueprintHolder::GetUI(AXActor* actor) const {
    if (const auto building = Cast<ABuilding>(actor))
        if (const auto condition = building->IsNonInteractable())
            return condition->GetSelectedUI();

    // TODO this is still special cases from the old way of doing it, it should be removed eventually
    if (actor->IsA(AConveyor::StaticClass())) {
        ConveyorUI->Conveyor = Cast<AConveyor>(actor);
        return ConveyorUI;
    }

    const auto controller = The::PlayerController(actor);

    if (const auto buildingActor = Cast<ABuilding>(actor))
        return CreateWidget<UBuildingSelectedUI>(controller, BuildingUIClass)->Init(buildingActor);

    if (const auto naturalResourceActor = Cast<ANaturalResourceActor>(actor))
        return CreateWidget<UNaturalResourceSelectedUI>(controller, NaturalResourceUIClass)->Init(naturalResourceActor);

    return nullptr;
}
