// Fill out your copyright notice in the Description page of Project Settings.

#include "Building.h"

#include "XD/Electricity/ElectricComponent.h"
#include "Substation.h"
#include "XD/GameInstanceX.h"

ABuilding::ABuilding() : constructionState(EConstructionState::BuilderMode) {}

void ABuilding::BeginPlay() {
    Super::BeginPlay();
}

void ABuilding::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}

void ABuilding::OnConstructionComplete(FConstructionFlags flags) {    
    constructionState = EConstructionState::Done;
    
    // if it has an Electric Component, try to connect it to a substation
    if (UElectricComponent* elec = FindComponentByClass<UElectricComponent>()) {
        bool connected = false;
        if (flags.autoConnectWires) {
            if (ASubstation* substation = GetGameInstance()->TheElectricityManager->FindNearestSubstation(GetActorLocation())) {
                substation->Connect(elec);
                connected = true;
            }
        }
        if (!connected)
            elec->SetState(PowerState::Disconnected);
    }
}

