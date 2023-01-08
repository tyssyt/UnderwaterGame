// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

ABuilding::ABuilding() : constructionState(ConstructionState::BuilderMode) {}

void ABuilding::BeginPlay() {
    Super::BeginPlay();
}

void ABuilding::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}
