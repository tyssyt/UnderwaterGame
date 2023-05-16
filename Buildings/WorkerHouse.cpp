// Fill out your copyright notice in the Description page of Project Settings.

#include "WorkerHouse.h"
#include "Habitat.h"

AWorkerHouse::AWorkerHouse() {

    // Structure to hold one-time initialization
    struct FConstructorStatics {
        ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
        FConstructorStatics() : PlaneMesh(TEXT("/Game/Assets/Meshes/WorkerHouse")) {}
    };
    static FConstructorStatics ConstructorStatics;

    // Create static mesh component
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
}

void AWorkerHouse::BeginPlay() {
    Super::BeginPlay();
}


const std::vector<Coordinate> OFFSETS_NORTH = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
const std::vector<Coordinate> OFFSETS_EAST = {{0, 0}, {1, 0}, {0, -1}, {1, -1}};
const std::vector<Coordinate> OFFSETS_SOUTH = {{0, 0}, {-1, 0}, {0, -1}, {-1, -1}};
const std::vector<Coordinate> OFFSETS_WEST = {{0, 0}, {-1, 0}, {0, 1}, {-1, 1}};

const std::vector<Coordinate>* AWorkerHouse::getGridOffsets() {
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



void UWorkerHouseUI::Tick() {
    if (!House)
        return;

    People->Set(House->Residents, AWorkerHouse::RESIDENT_LIMIT, House->GetGameInstance()->TheResourceBook->People);
    Workforce->SetText(FText::AsNumber(House->Residents)); // TODO update once we have a different formula for Workforce
}
