// Fill out your copyright notice in the Description page of Project Settings.

#include "DismantleMode.h"

#include "DismantleSite.h"
#include "The.h"
#include "XD/PlayerControllerX.h"
#include "XD/Buildings/Habitat.h"
#include "XD/Buildings/Splitter.h"

UDismantleMode* UDismantleMode::Init() {
    PreInit();
    const auto playerController = The::PlayerController(this);

    // TODO do we want to put anything in the SelectedUI?
    playerController->Deselect();

    // bind keys
    const auto inputComponent = playerController->InputComponent;
    inputComponent->BindAction("Select", IE_Pressed, this, &UDismantleMode::Confirm);
    return this;
}

bool UDismantleMode::Tick(const ACameraPawn& camera) {
    if (Done)
        return true;

    const auto underCursor = The::PlayerController(this)->GetUnderCursor<ABuilding>();
    if (underCursor != Highlight)
        SetHighlight(underCursor);
    return false;
}

void UDismantleMode::Stop(bool cancelled) {
    if (Done)
        return;    
    Done = true;

    SetHighlight(nullptr);

    const auto playerController = The::PlayerController(this);
    playerController->Deselect();

    // unbind
    playerController->InputComponent->RemoveActionBinding("Select", IE_Pressed);
}

void UDismantleMode::Confirm() {
    if (Done || !Highlight || !HighlightValid)
        return;

    const auto building = Highlight;
    Stop(false);

    NewObject<UDismantleSite>(building)->Init(building)->QueueTasks();
}

void UDismantleMode::SetHighlight(ABuilding* building) {
    if (Highlight) {
        Highlight->RemoveCondition(HighlightValid ? HighlightedInvalid : HighlightedOption);
        Highlight = nullptr;
    }

    if (building) {
        HighlightValid = CanBeDismantled(building);        
        building->AddCondition(HighlightValid ? HighlightedInvalid : HighlightedOption);
        Highlight = building;
    }
}

bool UDismantleMode::CanBeDismantled(ABuilding* building) {
    if (building->IsNonInteractable())
        return false;

    // splitter & mergers are invalid    
    if (building->IsA<AJunction>()) {
        return false;
    }

    // conveyors starting in mergers or ending in splitters are invalid
    if (const auto conveyor = Cast<AConveyor>(building)) {
        if (conveyor->Source->IsA<AMerger>() || conveyor->Target->IsA<ASplitter>())
            return false;
    }

    // buildings with attached conveyors are invalid
    if (const auto inventory = building->FindComponentByClass<UInventoryComponent>()) {
        // TODO allow partial conveyors and just remove the first/last link        
        for (int idx = 0, num = inventory->GetInputs().Num(); idx<num; idx++)
            if (inventory->GetConveyor(true, idx))
                return false;
        for (int idx = 0, num = inventory->GetOutputs().Num(); idx<num; idx++)
            if (inventory->GetConveyor(false, idx))
                return false;
    }

    // habitats with buildings in them are invalid
    if (const auto habitat = Cast<AHabitat>(building)) {
        if (!habitat->Buildings.IsEmpty())
            return false;
    }

    return true;
}
