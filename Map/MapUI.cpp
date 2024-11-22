// Fill out your copyright notice in the Description page of Project Settings.

#include "MapUI.h"

#include "The.h"
#include "Components/CanvasPanelSlot.h"
#include "XD/BlueprintHolder.h"

UMapUI* UMapUI::Init() {
    MapGrid->Init();
    return this;
}

void UMapUI::Open() {
    const auto slot = The::BPHolder(this)->MainUI->Canvas->AddChildToCanvas(this);
    // slot->SetSize({550,550});
    slot->SetAutoSize(true);
    slot->SetAnchors(FAnchors(0.5));
    slot->SetAlignment({0.5, 0.5});
    GetOwningPlayer()->InputComponent->BindAction("Deselect", IE_Pressed, this, &UMapUI::Close);
}
void UMapUI::Close() {
    MapGrid->Deselect();
    RemoveFromParent();
    GetOwningPlayer()->InputComponent->RemoveActionBinding("Deselect", IE_Pressed);
}

void UMapUI::RevealSector(int column, int row) const { MapGrid->RevealSector(column, row); }

void UMapUI::AddScout(UScout* scout) { MapGrid->AddScout(scout); }
void UMapUI::AddEvent(UEventInstance* event) { MapGrid->AddEvent(event); }
void UMapUI::UpdateWaypoints(UScout* scout) { MapGrid->UpdateWaypoints(scout); }

void UMapUI::UpdateMousePos(int column, int row) const {
    Subscript->SetText(FText::FromString(UMap::CoordinateToString(column, row)));
}
