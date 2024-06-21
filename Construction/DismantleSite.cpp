// Fill out your copyright notice in the Description page of Project Settings.

#include "DismantleSite.h"

#include "The.h"
#include "XD/Buildings/Habitat.h"
#include "XD/Buildings/IndoorBuilding.h"

UGettingDismantled::UGettingDismantled() {    
    const static ConstructorHelpers::FObjectFinder<UMaterialInstance> GhostMaterialFinder(TEXT("/Game/Assets/Materials/GhostMaterials/GhostMaterial_Grey"));
    Material = GhostMaterialFinder.Object;
    Type = EType::NonInteractable;
    // TODO dismantle UI?    
}

UDismantleSite* UDismantleSite::Init(ABuilding* building) {
    Building = building;
    building->AddCondition(NewObject<UGettingDismantled>(this)->WithSource(this));
    building->OnDismantleStart();
    return this;
}

void UDismantleSite::QueueTasks() {
    if (The::Cheats(this)->InstantBuild) {
        FinishDismantle();
        return;
    }

    if (Building->IsA<AIndoorBuilding>()) {
        // TODO wait dismantle time
        FinishDismantle();
        return;
    }

    Task = NewObject<UDismantleTask>(this)->Init(this);
    The::ConstructionManager(this)->AddTask(Task);
}

void UDismantleSite::FinishDismantle() const {
    Building->OnDismantleFinish();
    Building->Destroy();
}

UDismantleTask* UDismantleTask::Init(UDismantleSite* dismantleSite) {
    DismantleSite = dismantleSite;
    Location = DismantleSite->Building->GetActorLocation();
    return this;
}

ABuilderShip::FCommand UDismantleTask::GetNextCommand() {
    switch (NextCommand) {
    case EState::FlyToSite:
        NextCommand = EState::Done;
        return ABuilderShip::FCommand(DismantleSite->Building->GetActorLocation());
    // TODO wait dismantle time
    case EState::Done:
        DismantleSite->FinishDismantle();
        return ABuilderShip::FCommand();
    }
    checkNoEntry();
    return ABuilderShip::FCommand();
}
