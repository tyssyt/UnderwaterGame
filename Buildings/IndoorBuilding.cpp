// Fill out your copyright notice in the Description page of Project Settings.

#include "IndoorBuilding.h"
#include "Habitat.h"

// Sets default values
AIndoorBuilding::AIndoorBuilding() : GridX(-1), GridY(-1), Orientation(EGridOrientation::North) {
    PrimaryActorTick.bCanEverTick = true;
}

AIndoorBuilding* AIndoorBuilding::Init(UConstructionPlan* constructionPlan) {
    Super::Init(constructionPlan);
    return this;
}

void AIndoorBuilding::OnDismantleFinish() {
    Habitat->RemoveBuilding(this);
    Super::OnDismantleFinish();
}

void AIndoorBuilding::Rotate() {
    int i = static_cast<int>(Orientation);
    i = (i + 1) % 4;
    Orientation = static_cast<EGridOrientation>(i);
}

TArray<Coordinate>* AIndoorBuilding::GetGridOffsets() {
    return &ConstructionPlan->Shape[static_cast<int>(Orientation)];
}

void AIndoorBuilding::SetCoordinates(int x, int y, AHabitat* habitat) {
    GridX = x;
    GridY = y;
    Habitat = habitat;

    // not the most efficient way to compute the center, especially for larger buildings, but it gets the job done
    FVector center = FVector::ZeroVector;
    for (const Coordinate coord : *GetGridOffsets()) {
        center += habitat->GetGridCellCenter(x + coord.Key, y + coord.Value);
    }
    SetActorLocation(center / GetGridOffsets()->Num());

    FRotator rot = habitat->GetActorRotation();
    rot.Yaw += static_cast<int>(Orientation) * 90.f;
    SetActorRotation(rot);
}
