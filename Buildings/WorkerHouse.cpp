// Fill out your copyright notice in the Description page of Project Settings.

#include "WorkerHouse.h"
#include "Habitat.h"
#include "The.h"
#include "Components/VerticalBoxSlot.h"
#include "XD/BlueprintHolder.h"
#include "XD/Encyclopedia/Encyclopedia.h"

AWorkerHouse::AWorkerHouse() {
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/WorkerHouse"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRenderCustomDepth(true);
    SetRootComponent(Mesh);
    
    // Electricity = CreateDefaultSubobject<UElectricComponent>(TEXT("Electricity"));
    // Electricity->Consumption = 20;
}


const TArray<Coordinate> OFFSETS_NORTH = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
const TArray<Coordinate> OFFSETS_EAST = {{0, 0}, {1, 0}, {0, -1}, {1, -1}};
const TArray<Coordinate> OFFSETS_SOUTH = {{0, 0}, {-1, 0}, {0, -1}, {-1, -1}};
const TArray<Coordinate> OFFSETS_WEST = {{0, 0}, {-1, 0}, {0, 1}, {-1, 1}};

const TArray<Coordinate>* AWorkerHouse::GetGridOffsets() {
    switch (Orientation) {
    case EGridOrientation::North:
        return &OFFSETS_NORTH;
    case EGridOrientation::East:
        return &OFFSETS_EAST;
    case EGridOrientation::West:
        return &OFFSETS_SOUTH;
    case EGridOrientation::South:
        return &OFFSETS_WEST;
    }
    return nullptr;
}

void AWorkerHouse::InitSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) {
    components.Add(NewObject<UWorkerHouseUI>(this)->Init(this));    
    Super::InitSelectedUI(components);
}

UWorkerHouseUI* UWorkerHouseUI::Init(AWorkerHouse* house) {
    House = house;
    return this;
}

void UWorkerHouseUI::CreateUI(UBuildingSelectedUI* selectedUI) {
    Needs = CreateWidget<UNeedsSummaryUI>(selectedUI, The::BPHolder(House)->NeedsSummaryUIClass);
    const auto needsSlot = selectedUI->Content->AddChildToVerticalBox(Needs);
    needsSlot->SetHorizontalAlignment(HAlign_Center);

    People = CreateWidget<UInventorySlotUI>(selectedUI, The::BPHolder(House)->InventorySlotUIClass);
    People->Set(House->Residents, AWorkerHouse::RESIDENT_LIMIT, The::Encyclopedia(House)->People);
    const auto peopleSlot = selectedUI->Content->AddChildToVerticalBox(People);
    peopleSlot->SetHorizontalAlignment(HAlign_Center);
}

void UWorkerHouseUI::Tick(UBuildingSelectedUI* selectedUI) {
    Needs->SetNeeds(House->Habitat->PopulationManager);
    People->Set(House->Residents, AWorkerHouse::RESIDENT_LIMIT, The::Encyclopedia(House)->People);    
}
