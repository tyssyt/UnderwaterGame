// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionSite.h"

#include "ConstructionManager.h"
#include "The.h"

UUnderConstruction::UUnderConstruction() {
    const static ConstructorHelpers::FObjectFinder<UMaterial> GhostMaterialFinder(TEXT("/Game/Assets/Materials/GhostMaterials/GhostMaterial"));
    Material = GhostMaterialFinder.Object;
    Type = EType::NonInteractable;
    // TODO construction UI
}

UConstructionSite* UConstructionSite::Init(ABuilding* building, const UConstructionPlan* constructionPlan, UBuilderModeExtensions* extensions) {
    return Init(building, constructionPlan->Time, constructionPlan->Materials, extensions);
}

UConstructionSite* UConstructionSite::Init(ABuilding* building, int time, const TArray<Material>& materials, UBuilderModeExtensions* extensions) {
    Building = building;
    Time = time;
    Materials = materials;
    Extensions = extensions;

    Condition = NewObject<UUnderConstruction>(this);
    building->AddCondition(Condition);
    return this;
}

void UConstructionSite::QueueTasks() {
    if (The::Cheats(this)->InstantBuild) {
        FinishConstruction();
        return;
    }

    const auto constructionManager = The::ConstructionManager(this);
    for (const auto& material : Materials) {
        const auto task = NewObject<UDeliverResource>(this)->Init(this, material);
        Tasks.Add(task);
        constructionManager->AddTask(task);
    }
}

void UConstructionSite::FinishConstruction() const {
    Building->RemoveCondition(Condition);
    Building->OnConstructionComplete(Extensions);
}

void UDeliverResource::PickupMaterial() const {
    const auto input = PickupFrom->Inventory->GetInput(RequiredMaterial.resource);
    check(input && input->Current >= RequiredMaterial.amount);
    input->PullFrom(RequiredMaterial.amount);
    PickupFrom->Unreserve(RequiredMaterial);
}

UDeliverResource* UDeliverResource::Init(UConstructionSite* constructionSite, const Material& material) {
    ConstructionSite = constructionSite;
    Location = constructionSite->Building->GetActorLocation();
    RequiredMaterial = material;
    return this;
}

ABuilderShip::FCommand UDeliverResource::GetNextCommand() {
    switch (NextCommand) {
    case EState::Collect:
        NextCommand = EState::Deliver;
        return ABuilderShip::FCommand(PickupFrom->GetActorLocation());
    case EState::Deliver:
        NextCommand = EState::Construct;
        PickupMaterial();
        return ABuilderShip::FCommand(ConstructionSite->Building->GetActorLocation());
    case EState::Construct:
        NextCommand = EState::Done;
        ConstructionSite->Tasks.Remove(this);
        if (ConstructionSite->Tasks.IsEmpty())
            return ABuilderShip::FCommand(ConstructionSite->Time);
        return ABuilderShip::FCommand();
    case EState::Done:
        ConstructionSite->FinishConstruction();
        return ABuilderShip::FCommand();
    }
    checkNoEntry();
    return ABuilderShip::FCommand();
}
